#pragma once
#include <GLFW/glfw3.h>

namespace xac {
class ImguiLayer {
 public:
  ImguiLayer();
  auto Init(GLFWwindow *window) -> void;
};
}  // namespace xac
