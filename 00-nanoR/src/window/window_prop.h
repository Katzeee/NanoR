#pragma once
namespace nanoR {

struct WindowProp {
  unsigned int width;
  unsigned int height;
  const char *title;

  WindowProp() : WindowProp(2000, 900, "Test") {}
  WindowProp(unsigned int width, unsigned int height, const char *title) : width(width), height(height), title(title) {}
};
}  // namespace nanoR