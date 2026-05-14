#include <string>

#include <kromakit/Label.h>
#include <kromakit/Panel.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include "TestHarness.h"

namespace {

int g_styleNameCounter = 0;

DUIStyleName UniqueStyleName(const std::string& base) {
  ++g_styleNameCounter;
  return DUIStyleName(base + "." + std::to_string(g_styleNameCounter));
}

}  // namespace

TEST_CASE(StyleRegistry_RegisterAndLookupStyle) {
  const DUIStyleName styleName = UniqueStyleName("tests.style.lookup");
  EXPECT_FALSE(DUIStyleRegistry::Exists(styleName));

  DUIStyleRegistry::Register<Label>(styleName, [](Label& label) {
    label.sContent = "registered";
    label.isHitTestVisible = false;
  });

  EXPECT_TRUE(DUIStyleRegistry::Exists(styleName));

  Label label;
  EXPECT_EQ(label.sContent, std::string(""));
  EXPECT_TRUE(label.isHitTestVisible);

  EXPECT_TRUE(DUIStyleRegistry::Apply(styleName, label));
  EXPECT_EQ(label.sContent, std::string("registered"));
  EXPECT_FALSE(label.isHitTestVisible);

  const DUIStyleName missing = UniqueStyleName("tests.style.missing");
  EXPECT_FALSE(DUIStyleRegistry::Exists(missing));
  EXPECT_FALSE(DUIStyleRegistry::Apply(missing, label));
}

TEST_CASE(StyleRegistry_DuplicateRegistrationKeepsFirst) {
  const DUIStyleName styleName = UniqueStyleName("tests.style.duplicate");

  DUIStyleRegistry::Register<Label>(styleName, [](Label& label) {
    label.sContent = "first";
  });

  // Intentionally triggers the registry's duplicate-registration log.
  DUIStyleRegistry::Register<Label>(styleName, [](Label& label) {
    label.sContent = "second";
  });

  Label label;
  EXPECT_TRUE(DUIStyleRegistry::Apply(styleName, label));
  EXPECT_EQ(label.sContent, std::string("first"));
}

TEST_CASE(StyleRegistry_StyleNameNormalizationRoundTrip) {
  const DUIStyleName raw("  tests . STYLE . Name  ");
  const DUIStyleName normalized("tests.style.name");

  DUIStyleRegistry::Register<Label>(raw, [](Label& label) {
    label.sContent = "normalized";
  });

  EXPECT_TRUE(DUIStyleRegistry::Exists(normalized));

  Label label;
  EXPECT_TRUE(DUIStyleRegistry::Apply(normalized, label));
  EXPECT_EQ(label.sContent, std::string("normalized"));
}

TEST_CASE(StyleRegistry_ApplyPersistsVisualProperties) {
  const DUIStyleName styleName = UniqueStyleName("tests.style.props");

  DUIStyleRegistry::Register<Panel>(styleName, [](Panel& panel) {
    panel.cornerRadius = DUIInsets(1.0f, 2.0f, 3.0f, 4.0f);
    panel.margin = DUIInsets(5.0f, 6.0f, 7.0f, 8.0f);
    panel.background = ColorFromRGB(10, 20, 30, 200);
  });

  Panel panel;
  EXPECT_TRUE(DUIStyleRegistry::Apply(styleName, panel));

  EXPECT_EQ(panel.cornerRadius, DUIInsets(1.0f, 2.0f, 3.0f, 4.0f));
  EXPECT_EQ(panel.margin, DUIInsets(5.0f, 6.0f, 7.0f, 8.0f));

  EXPECT_NEAR(panel.background.r, 10.0f / 255.0f, 0.0001f);
  EXPECT_NEAR(panel.background.g, 20.0f / 255.0f, 0.0001f);
  EXPECT_NEAR(panel.background.b, 30.0f / 255.0f, 0.0001f);
  EXPECT_NEAR(panel.background.a, 200.0f / 255.0f, 0.0001f);
}

TEST_CASE(StyleRegistry_TypedApplyMismatchReturnsFalse) {
  const DUIStyleName styleName = UniqueStyleName("tests.style.typed");

  DUIStyleRegistry::Register<Label>(styleName, [](Label& label) {
    label.sContent = "typed";
  });

  Panel panel;
  // Intentionally applies a Label style to Panel to exercise typed mismatch logging.
  EXPECT_FALSE(DUIStyleRegistry::Apply(styleName, panel));
}

TEST_CASE(StyleRegistry_ComposedStyleViaUseStyle) {
  const DUIStyleName baseStyle = UniqueStyleName("tests.style.base");
  const DUIStyleName derivedStyle = UniqueStyleName("tests.style.derived");

  DUIStyleRegistry::Register<Panel>(baseStyle, [](Panel& panel) {
    panel.margin = DUIInsets(4.0f);
    panel.background = ColorFromRGB(120, 100, 80, 255);
  });

  DUIStyleRegistry::Register<Panel>(derivedStyle, [baseStyle](Panel& panel) {
    panel.UseStyle(baseStyle);
    panel.isHitTestVisible = false;
  });

  Panel panel;
  EXPECT_TRUE(panel.UseStyle(derivedStyle));
  EXPECT_EQ(panel.margin, DUIInsets(4.0f));
  EXPECT_FALSE(panel.isHitTestVisible);
  EXPECT_EQ(panel.GetStyleName().ToString(), derivedStyle.ToString());
  EXPECT_NEAR(panel.background.r, 120.0f / 255.0f, 0.0001f);
}
