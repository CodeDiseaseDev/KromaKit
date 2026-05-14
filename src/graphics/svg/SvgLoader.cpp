//
// Created by code on 5/8/26.
//

#include <kromakit/graphics/svg/SvgLoader.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

namespace {

using StyleProperties = std::unordered_map<std::string, std::string>;

static std::string_view TrimView(std::string_view value) {
  size_t begin = 0;
  while (begin < value.size() && std::isspace(static_cast<unsigned char>(value[begin])))
    ++begin;

  size_t end = value.size();
  while (end > begin && std::isspace(static_cast<unsigned char>(value[end - 1])))
    --end;

  return value.substr(begin, end - begin);
}

static std::string ToLowerAscii(std::string_view value) {
  std::string lowered;
  lowered.reserve(value.size());
  for (char ch : value) {
    lowered.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
  }
  return lowered;
}

static bool IsSimpleClassSelector(std::string_view selector, std::string* classNameOut) {
  selector = TrimView(selector);
  if (selector.size() < 2 || selector[0] != '.')
    return false;

  for (size_t i = 1; i < selector.size(); ++i) {
    const char ch = selector[i];
    const bool valid =
      std::isalnum(static_cast<unsigned char>(ch)) ||
      ch == '_' ||
      ch == '-';
    if (!valid)
      return false;
  }

  if (classNameOut) {
    *classNameOut = ToLowerAscii(selector.substr(1));
  }
  return true;
}

static bool IsStylePropertySupported(std::string_view propertyName) {
  static const std::unordered_set<std::string> supported = {
    "display",
    "opacity",
    "fill",
    "fill-rule",
    "fill-opacity",
    "stroke",
    "stroke-width",
    "stroke-opacity",
    "stroke-linecap",
    "stroke-linejoin",
    "stroke-miterlimit"
  };

  return supported.find(ToLowerAscii(propertyName)) != supported.end();
}

static void ParseStyleDeclarations(
  std::string_view declarations,
  StyleProperties* out) {

  if (!out)
    return;

  size_t pos = 0;
  while (pos < declarations.size()) {
    const size_t nextSemi = declarations.find(';', pos);
    const size_t itemEnd = (nextSemi == std::string_view::npos)
      ? declarations.size()
      : nextSemi;

    const std::string_view item = TrimView(declarations.substr(pos, itemEnd - pos));
    if (!item.empty()) {
      const size_t colon = item.find(':');
      if (colon != std::string_view::npos) {
        const std::string property = ToLowerAscii(TrimView(item.substr(0, colon)));
        const std::string value = std::string(TrimView(item.substr(colon + 1)));

        if (!property.empty() && !value.empty() && IsStylePropertySupported(property))
          (*out)[property] = value;
      }
    }

    if (nextSemi == std::string_view::npos)
      break;

    pos = nextSemi + 1;
  }
}

static void ParseClassStyleRules(
  std::string_view cssText,
  std::unordered_map<std::string, StyleProperties>* classRulesOut) {

  if (!classRulesOut)
    return;

  size_t cursor = 0;
  while (cursor < cssText.size()) {
    const size_t openBrace = cssText.find('{', cursor);
    if (openBrace == std::string_view::npos)
      break;

    const size_t closeBrace = cssText.find('}', openBrace + 1);
    if (closeBrace == std::string_view::npos)
      break;

    const std::string_view selectorList = TrimView(cssText.substr(cursor, openBrace - cursor));
    const std::string_view declarations = cssText.substr(openBrace + 1, closeBrace - openBrace - 1);

    StyleProperties parsedDeclarations;
    ParseStyleDeclarations(declarations, &parsedDeclarations);

    if (!parsedDeclarations.empty()) {
      size_t selectorPos = 0;
      while (selectorPos < selectorList.size()) {
        const size_t nextComma = selectorList.find(',', selectorPos);
        const size_t selectorEnd = (nextComma == std::string_view::npos)
          ? selectorList.size()
          : nextComma;

        const std::string_view selector = selectorList.substr(selectorPos, selectorEnd - selectorPos);
        std::string className;
        if (IsSimpleClassSelector(selector, &className)) {
          auto& targetProperties = (*classRulesOut)[className];
          for (const auto& [property, value] : parsedDeclarations)
            targetProperties[property] = value;
        }

        if (nextComma == std::string_view::npos)
          break;
        selectorPos = nextComma + 1;
      }
    }

    cursor = closeBrace + 1;
  }
}

struct AttrSpan {
  bool found = false;
  size_t nameStart = 0;
  size_t nameEnd = 0;
  size_t valueStart = 0;
  size_t valueEnd = 0;
  size_t quoteStart = 0;
  size_t quoteEnd = 0;
  char quote = '\0';
};

static AttrSpan FindAttribute(std::string_view tag, std::string_view attributeName) {
  const std::string loweredTarget = ToLowerAscii(attributeName);
  size_t pos = 0;

  if (pos < tag.size() && tag[pos] == '<')
    ++pos;

  while (pos < tag.size() && std::isspace(static_cast<unsigned char>(tag[pos])))
    ++pos;

  // Skip the element name token first.
  while (pos < tag.size()) {
    const char ch = tag[pos];
    if (std::isspace(static_cast<unsigned char>(ch)) || ch == '>' || ch == '/')
      break;
    ++pos;
  }

  while (pos < tag.size()) {
    while (pos < tag.size() && std::isspace(static_cast<unsigned char>(tag[pos])))
      ++pos;

    if (pos >= tag.size() || tag[pos] == '>' || tag[pos] == '/')
      break;

    const size_t nameStart = pos;
    while (pos < tag.size()) {
      const char ch = tag[pos];
      if (std::isspace(static_cast<unsigned char>(ch)) || ch == '=' || ch == '>' || ch == '/')
        break;
      ++pos;
    }
    const size_t nameEnd = pos;
    const std::string nameLower = ToLowerAscii(tag.substr(nameStart, nameEnd - nameStart));

    while (pos < tag.size() && std::isspace(static_cast<unsigned char>(tag[pos])))
      ++pos;

    if (pos >= tag.size() || tag[pos] != '=') {
      while (pos < tag.size() && tag[pos] != '>' && tag[pos] != '/')
        ++pos;
      continue;
    }

    ++pos;
    while (pos < tag.size() && std::isspace(static_cast<unsigned char>(tag[pos])))
      ++pos;

    AttrSpan span;
    span.nameStart = nameStart;
    span.nameEnd = nameEnd;

    if (pos < tag.size() && (tag[pos] == '"' || tag[pos] == '\'')) {
      span.quote = tag[pos];
      span.quoteStart = pos;
      ++pos;
      span.valueStart = pos;
      while (pos < tag.size() && tag[pos] != span.quote)
        ++pos;
      span.valueEnd = pos;
      span.quoteEnd = (pos < tag.size()) ? pos : tag.size();
      if (pos < tag.size())
        ++pos;
    } else {
      span.valueStart = pos;
      while (pos < tag.size()) {
        const char ch = tag[pos];
        if (std::isspace(static_cast<unsigned char>(ch)) || ch == '>')
          break;
        ++pos;
      }
      span.valueEnd = pos;
      span.quoteStart = span.valueStart;
      span.quoteEnd = span.valueEnd;
    }

    if (nameLower == loweredTarget) {
      span.found = true;
      return span;
    }
  }

  return AttrSpan{};
}

static std::vector<std::string> ParseClassList(std::string_view classAttributeValue) {
  std::vector<std::string> classes;
  size_t pos = 0;
  while (pos < classAttributeValue.size()) {
    while (pos < classAttributeValue.size() &&
      std::isspace(static_cast<unsigned char>(classAttributeValue[pos])))
      ++pos;

    const size_t start = pos;
    while (pos < classAttributeValue.size() &&
      !std::isspace(static_cast<unsigned char>(classAttributeValue[pos])))
      ++pos;

    if (pos > start) {
      classes.push_back(ToLowerAscii(classAttributeValue.substr(start, pos - start)));
    }
  }
  return classes;
}

static std::unordered_set<std::string> ParseInlineStylePropertyNames(std::string_view styleValue) {
  std::unordered_set<std::string> names;
  size_t pos = 0;
  while (pos < styleValue.size()) {
    const size_t nextSemi = styleValue.find(';', pos);
    const size_t itemEnd = (nextSemi == std::string_view::npos) ? styleValue.size() : nextSemi;

    const std::string_view item = TrimView(styleValue.substr(pos, itemEnd - pos));
    if (!item.empty()) {
      const size_t colon = item.find(':');
      if (colon != std::string_view::npos) {
        const std::string property = ToLowerAscii(TrimView(item.substr(0, colon)));
        if (!property.empty())
          names.insert(property);
      }
    }

    if (nextSemi == std::string_view::npos)
      break;
    pos = nextSemi + 1;
  }
  return names;
}

static bool IsDrawableSvgElement(std::string_view elementName) {
  static const std::unordered_set<std::string> drawableElements = {
    "g",
    "path",
    "rect",
    "circle",
    "ellipse",
    "line",
    "polyline",
    "polygon"
  };
  return drawableElements.find(ToLowerAscii(elementName)) != drawableElements.end();
}

static bool ContainsAttributeName(std::string_view tag, std::string_view attributeName) {
  return FindAttribute(tag, attributeName).found;
}

static bool ReadFileToString(
  const std::string& filePath,
  std::string* outText) {

  if (!outText)
    return false;

  std::ifstream in(filePath, std::ios::binary);
  if (!in)
    return false;

  std::ostringstream buffer;
  buffer << in.rdbuf();
  *outText = buffer.str();
  return !outText->empty();
}

static std::unordered_map<std::string, StyleProperties> ExtractClassStyleRules(
  const std::string& svgText) {

  std::unordered_map<std::string, StyleProperties> classRules;
  const std::string lower = ToLowerAscii(svgText);

  size_t pos = 0;
  while (pos < lower.size()) {
    const size_t styleOpen = lower.find("<style", pos);
    if (styleOpen == std::string::npos)
      break;

    const size_t styleTagEnd = lower.find('>', styleOpen);
    if (styleTagEnd == std::string::npos)
      break;

    const size_t styleClose = lower.find("</style>", styleTagEnd + 1);
    if (styleClose == std::string::npos)
      break;

    const std::string_view cssBlock(svgText.data() + styleTagEnd + 1, styleClose - styleTagEnd - 1);
    ParseClassStyleRules(cssBlock, &classRules);

    pos = styleClose + 8;
  }

  return classRules;
}

static std::string BuildStyleAppendText(const StyleProperties& properties) {
  std::string styleText;
  for (const auto& [property, value] : properties) {
    if (property.empty() || value.empty())
      continue;
    styleText += property;
    styleText += ':';
    styleText += value;
    styleText += ';';
  }
  return styleText;
}

static std::string ApplyClassStylesToSvgText(
  const std::string& svgText,
  const std::unordered_map<std::string, StyleProperties>& classRules) {

  if (svgText.empty() || classRules.empty())
    return svgText;

  std::string output;
  output.reserve(svgText.size() + svgText.size() / 10);

  size_t cursor = 0;
  while (cursor < svgText.size()) {
    const size_t tagStart = svgText.find('<', cursor);
    if (tagStart == std::string::npos) {
      output.append(svgText, cursor, std::string::npos);
      break;
    }

    output.append(svgText, cursor, tagStart - cursor);

    const size_t tagEnd = svgText.find('>', tagStart);
    if (tagEnd == std::string::npos) {
      output.append(svgText, tagStart, std::string::npos);
      break;
    }

    std::string tag = svgText.substr(tagStart, tagEnd - tagStart + 1);
    if (tag.size() >= 2 && tag[1] != '/' && tag[1] != '!' && tag[1] != '?') {
      size_t nameStart = 1;
      while (nameStart < tag.size() && std::isspace(static_cast<unsigned char>(tag[nameStart])))
        ++nameStart;

      size_t nameEnd = nameStart;
      while (nameEnd < tag.size()) {
        const char ch = tag[nameEnd];
        if (std::isspace(static_cast<unsigned char>(ch)) || ch == '>' || ch == '/')
          break;
        ++nameEnd;
      }

      const std::string_view elementName(tag.data() + nameStart, nameEnd - nameStart);
      if (IsDrawableSvgElement(elementName)) {
        const AttrSpan classAttr = FindAttribute(tag, "class");
        if (classAttr.found && classAttr.valueEnd > classAttr.valueStart) {
          const std::string_view classValue(tag.data() + classAttr.valueStart, classAttr.valueEnd - classAttr.valueStart);
          const std::vector<std::string> classes = ParseClassList(classValue);

          StyleProperties mergedProperties;
          for (const auto& className : classes) {
            const auto ruleIt = classRules.find(className);
            if (ruleIt == classRules.end())
              continue;
            for (const auto& [property, value] : ruleIt->second)
              mergedProperties[property] = value;
          }

          if (!mergedProperties.empty()) {
            for (auto it = mergedProperties.begin(); it != mergedProperties.end(); ) {
              if (ContainsAttributeName(tag, it->first))
                it = mergedProperties.erase(it);
              else
                ++it;
            }

            if (!mergedProperties.empty()) {
              const AttrSpan styleAttr = FindAttribute(tag, "style");
              if (styleAttr.found && styleAttr.valueEnd >= styleAttr.valueStart) {
                const std::string_view styleValue(tag.data() + styleAttr.valueStart, styleAttr.valueEnd - styleAttr.valueStart);
                const std::unordered_set<std::string> existingStyleNames =
                  ParseInlineStylePropertyNames(styleValue);

                for (auto it = mergedProperties.begin(); it != mergedProperties.end(); ) {
                  if (existingStyleNames.find(it->first) != existingStyleNames.end())
                    it = mergedProperties.erase(it);
                  else
                    ++it;
                }

                if (!mergedProperties.empty()) {
                  std::string styleAppend = BuildStyleAppendText(mergedProperties);
                  if (!styleAppend.empty()) {
                    tag.insert(styleAttr.valueEnd, styleAppend);
                  }
                }
              } else {
                std::string styleAppend = BuildStyleAppendText(mergedProperties);
                if (!styleAppend.empty()) {
                  const size_t insertPos = (tag.size() >= 2 && tag[tag.size() - 2] == '/')
                    ? tag.size() - 2
                    : tag.size() - 1;
                  tag.insert(insertPos, " style=\"" + styleAppend + "\"");
                }
              }
            }
          }
        }
      }
    }

    output += tag;
    cursor = tagEnd + 1;
  }

  return output;
}

static NSVGimage* ParseSvgDocument(const std::string& filePath) {
  std::string svgText;
  if (!ReadFileToString(filePath, &svgText))
    return nsvgParseFromFile(filePath.c_str(), "px", 96.0f);

  const auto classRules = ExtractClassStyleRules(svgText);
  std::string preprocessed = ApplyClassStylesToSvgText(svgText, classRules);
  if (preprocessed.empty())
    preprocessed = svgText;

  std::vector<char> mutableBuffer(preprocessed.begin(), preprocessed.end());
  mutableBuffer.push_back('\0');
  return nsvgParse(mutableBuffer.data(), "px", 96.0f);
}

static NVGcolor ConvertNanoSvgColor(unsigned int color) {
  const auto r = static_cast<uint8_t>((color >> 0) & 0xff);
  const auto g = static_cast<uint8_t>((color >> 8) & 0xff);
  const auto b = static_cast<uint8_t>((color >> 16) & 0xff);

  return nvgRGBA(r, g, b, 255);
}

static SvgPaintType ConvertPaintType(signed char paintType) {
  switch (paintType) {
    case NSVG_PAINT_NONE:
      return SvgPaintType::None;

    case NSVG_PAINT_COLOR:
      return SvgPaintType::Color;

    default:
      return SvgPaintType::Unsupported;
  }
}

static SvgFillRule ConvertFillRule(char fillRule) {
  if (fillRule == NSVG_FILLRULE_EVENODD)
    return SvgFillRule::EvenOdd;
  return SvgFillRule::NonZero;
}

static SvgLineCap ConvertLineCap(char cap) {
  switch (cap) {
    case NSVG_CAP_ROUND:
      return SvgLineCap::Round;

    case NSVG_CAP_SQUARE:
      return SvgLineCap::Square;

    case NSVG_CAP_BUTT:
    default:
      return SvgLineCap::Butt;
  }
}

static SvgLineJoin ConvertLineJoin(char join) {
  switch (join) {
    case NSVG_JOIN_ROUND:
      return SvgLineJoin::Round;

    case NSVG_JOIN_BEVEL:
      return SvgLineJoin::Bevel;

    case NSVG_JOIN_MITER:
    default:
      return SvgLineJoin::Miter;
  }
}

static float ClampUnit(float value) {
  return std::max(0.0f, std::min(1.0f, value));
}

static float CubicAt(float p0, float p1, float p2, float p3, float t) {
  const float u = 1.0f - t;
  const float uu = u * u;
  const float tt = t * t;
  return
    (uu * u) * p0 +
    (3.0f * uu * t) * p1 +
    (3.0f * u * tt) * p2 +
    (tt * t) * p3;
}

static std::vector<float> FlattenSubPath(
  const SvgSubPath& path,
  bool closeForFill,
  int samplesPerCurve = 12) {

  std::vector<float> polygon;
  if (!closeForFill || path.points.size() < 8)
    return polygon;

  const int pointCount = static_cast<int>(path.points.size() / 2);
  if (pointCount < 4)
    return polygon;

  polygon.reserve(static_cast<size_t>(pointCount) * static_cast<size_t>(samplesPerCurve));
  polygon.push_back(path.points[0]);
  polygon.push_back(path.points[1]);

  for (int i = 0; i + 3 < pointCount; i += 3) {
    const float p0x = path.points[(i + 0) * 2 + 0];
    const float p0y = path.points[(i + 0) * 2 + 1];
    const float p1x = path.points[(i + 1) * 2 + 0];
    const float p1y = path.points[(i + 1) * 2 + 1];
    const float p2x = path.points[(i + 2) * 2 + 0];
    const float p2y = path.points[(i + 2) * 2 + 1];
    const float p3x = path.points[(i + 3) * 2 + 0];
    const float p3y = path.points[(i + 3) * 2 + 1];

    for (int step = 1; step <= samplesPerCurve; ++step) {
      const float t = static_cast<float>(step) / static_cast<float>(samplesPerCurve);
      polygon.push_back(CubicAt(p0x, p1x, p2x, p3x, t));
      polygon.push_back(CubicAt(p0y, p1y, p2y, p3y, t));
    }
  }

  return polygon;
}

static bool PointInPolygon(
  const std::vector<float>& polygon,
  float x, float y) {

  const int pointCount = static_cast<int>(polygon.size() / 2);
  if (pointCount < 3)
    return false;

  bool inside = false;
  for (int i = 0, j = pointCount - 1; i < pointCount; j = i++) {
    const float xi = polygon[i * 2 + 0];
    const float yi = polygon[i * 2 + 1];
    const float xj = polygon[j * 2 + 0];
    const float yj = polygon[j * 2 + 1];

    const bool intersects = ((yi > y) != (yj > y)) &&
      (x < (xj - xi) * (y - yi) / ((yj - yi) + 1e-12f) + xi);
    if (intersects)
      inside = !inside;
  }

  return inside;
}

struct SamplePoint {
  float x = 0.0f;
  float y = 0.0f;
};

static void GenerateCandidatePoints(
  const std::vector<float>& polygon,
  std::vector<SamplePoint>* out) {

  if (!out)
    return;

  const int pointCount = static_cast<int>(polygon.size() / 2);
  if (pointCount < 3)
    return;

  float minX = polygon[0];
  float maxX = polygon[0];
  float minY = polygon[1];
  float maxY = polygon[1];

  for (int i = 1; i < pointCount; ++i) {
    minX = std::min(minX, polygon[i * 2 + 0]);
    maxX = std::max(maxX, polygon[i * 2 + 0]);
    minY = std::min(minY, polygon[i * 2 + 1]);
    maxY = std::max(maxY, polygon[i * 2 + 1]);
  }

  const float centerX = (minX + maxX) * 0.5f;
  const float centerY = (minY + maxY) * 0.5f;

  float areaTwice = 0.0f;
  float centroidX = 0.0f;
  float centroidY = 0.0f;
  for (int i = 0, j = pointCount - 1; i < pointCount; j = i++) {
    const float x0 = polygon[j * 2 + 0];
    const float y0 = polygon[j * 2 + 1];
    const float x1 = polygon[i * 2 + 0];
    const float y1 = polygon[i * 2 + 1];
    const float cross = x0 * y1 - x1 * y0;
    areaTwice += cross;
    centroidX += (x0 + x1) * cross;
    centroidY += (y0 + y1) * cross;
  }

  out->clear();
  out->reserve(96);
  out->push_back({centerX, centerY});

  if (std::fabs(areaTwice) > 1e-6f) {
    out->push_back({
      centroidX / (3.0f * areaTwice),
      centroidY / (3.0f * areaTwice)
    });
  }

  const float quarterX = (maxX - minX) * 0.25f;
  const float quarterY = (maxY - minY) * 0.25f;
  out->push_back({centerX - quarterX, centerY});
  out->push_back({centerX + quarterX, centerY});
  out->push_back({centerX, centerY - quarterY});
  out->push_back({centerX, centerY + quarterY});

  for (int gx = 1; gx <= 5; ++gx) {
    for (int gy = 1; gy <= 5; ++gy) {
      const float x = minX + (maxX - minX) * (static_cast<float>(gx) / 6.0f);
      const float y = minY + (maxY - minY) * (static_cast<float>(gy) / 6.0f);
      out->push_back({x, y});
    }
  }

  for (int i = 0; i < pointCount; ++i) {
    const int j = (i + 1) % pointCount;
    const float x0 = polygon[i * 2 + 0];
    const float y0 = polygon[i * 2 + 1];
    const float x1 = polygon[j * 2 + 0];
    const float y1 = polygon[j * 2 + 1];
    const float midX = (x0 + x1) * 0.5f;
    const float midY = (y0 + y1) * 0.5f;

    // Pull edge midpoint slightly toward center to avoid boundary ambiguity.
    out->push_back({
      midX * 0.9f + centerX * 0.1f,
      midY * 0.9f + centerY * 0.1f
    });

    // Pull vertex toward center as another interior-biased sample.
    out->push_back({
      x0 * 0.85f + centerX * 0.15f,
      y0 * 0.85f + centerY * 0.15f
    });
  }
}

static int ComputeMinimumContainingCount(
  const std::vector<float>& polygon,
  size_t polygonIndex,
  const std::vector<std::vector<float>>& allPolygons) {

  std::vector<SamplePoint> candidates;
  GenerateCandidatePoints(polygon, &candidates);

  int minContaining = std::numeric_limits<int>::max();
  for (const auto& candidate : candidates) {
    if (!PointInPolygon(polygon, candidate.x, candidate.y))
      continue;

    int containingCount = 0;
    for (size_t j = 0; j < allPolygons.size(); ++j) {
      if (j == polygonIndex || allPolygons[j].size() < 6)
        continue;

      if (PointInPolygon(allPolygons[j], candidate.x, candidate.y))
        ++containingCount;
    }

    minContaining = std::min(minContaining, containingCount);
  }

  return minContaining;
}

static void ComputeEvenOddHoles(SvgShape& shape) {
  for (auto& path : shape.paths)
    path.isHole = false;

  if (!shape.hasFill || shape.fillRule != SvgFillRule::EvenOdd)
    return;

  const size_t pathCount = shape.paths.size();
  std::vector<std::vector<float>> flattened(pathCount);

  for (size_t i = 0; i < pathCount; ++i) {
    // SVG fill semantics implicitly close open subpaths for filling.
    flattened[i] = FlattenSubPath(shape.paths[i], true);
  }

  for (size_t i = 0; i < pathCount; ++i) {
    if (flattened[i].size() < 6)
      continue;

    const int minContainingCount =
      ComputeMinimumContainingCount(flattened[i], i, flattened);
    if (minContainingCount == std::numeric_limits<int>::max())
      continue;

    shape.paths[i].isHole = (minContainingCount % 2) != 0;
  }
}

} // namespace

std::shared_ptr<SvgDocument> SvgLoader::LoadFromFile(
  const std::string& file_path) {

  NSVGimage* image = ParseSvgDocument(file_path);
  if (!image)
    return nullptr;

  auto doc = std::make_shared<SvgDocument>();
  doc->width = image->width;
  doc->height = image->height;

  for (NSVGshape* shape = image->shapes; shape != nullptr; shape = shape->next) {
    SvgShape outShape;

    outShape.visible = (shape->flags & NSVG_FLAGS_VISIBLE) != 0;
    outShape.opacity = ClampUnit(shape->opacity);
    outShape.fillRule = ConvertFillRule(shape->fillRule);
    outShape.paintOrder = shape->paintOrder;
    outShape.fillType = ConvertPaintType(shape->fill.type);
    outShape.strokeType = ConvertPaintType(shape->stroke.type);

    outShape.hasFill = outShape.fillType == SvgPaintType::Color;
    outShape.hasStroke =
      outShape.strokeType == SvgPaintType::Color &&
      shape->strokeWidth > 0.0f;

    outShape.strokeWidth = shape->strokeWidth;
    outShape.strokeLineCap = ConvertLineCap(shape->strokeLineCap);
    outShape.strokeLineJoin = ConvertLineJoin(shape->strokeLineJoin);
    outShape.miterLimit = shape->miterLimit;

    if (outShape.hasFill)
      outShape.fillColor = ConvertNanoSvgColor(shape->fill.color);

    if (outShape.hasStroke)
      outShape.strokeColor = ConvertNanoSvgColor(shape->stroke.color);

    for (NSVGpath* path = shape->paths; path != nullptr; path = path->next) {
      SvgSubPath outPath;
      outPath.closed = path->closed != 0;
      outPath.points.assign(path->pts, path->pts + path->npts * 2);

      outShape.paths.push_back(std::move(outPath));
    }

    ComputeEvenOddHoles(outShape);

    doc->shapes.push_back(std::move(outShape));
  }

  nsvgDelete(image);
  return doc;
}

// std::unique_ptr<SvgDocument> SvgLoader::LoadFromString(
//   const std::string &svgText) {
//
//
// }
