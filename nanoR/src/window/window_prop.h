#pragma once
namespace nanoR {

struct WindowProp {
  unsigned int height;
  unsigned int width;
  const char *title;

  WindowProp() : WindowProp(900, 1600, "Test") {}
  WindowProp(unsigned int height, unsigned int width, const char *title) : height(height), width(width), title(title) {}
};
}  // namespace nanoR