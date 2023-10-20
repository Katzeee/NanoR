#pragma once

namespace nanoR {
struct WindowProp {
  unsigned int height;
  unsigned int width;
  const char *title;

  WindowProp() : WindowProp(900, 1600, "Test") {}
  WindowProp(unsigned int height, unsigned int width, const char *title) : height(height), width(width), title(title) {}
};

class Window {
 public:
  Window() = default;
  virtual auto Init(WindowProp window_prop) -> void = 0;
  virtual auto Tick() -> void = 0;
  virtual auto Shutdown() -> void = 0;
  virtual ~Window() = default;

 protected:
};
}  // namespace nanoR