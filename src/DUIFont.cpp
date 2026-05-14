#include <kromakit/DUIFont.h>

DUIFont::DUIFont(
  std::string family, float size,
  FontWeight weight, FontStyle style) {

  this->Family = family;
  this->FontSize = size;
  this->Weight = weight;
  this->Style = style;
}
