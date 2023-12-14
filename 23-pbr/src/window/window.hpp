#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

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
