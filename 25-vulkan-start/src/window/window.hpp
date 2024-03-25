#pragma once

struct GLFWwindow;

namespace xac {
class Window {
 public:
  Window();
  ~Window();
  auto GetRawWindow() -> GLFWwindow *;
  auto ShouldClose() -> bool;
  auto SwapBuffers() -> void;

 private:
  GLFWwindow *window_;
};
}  // namespace xac
