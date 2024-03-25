#include "window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

namespace xac {

class Camera;

Window::Window() {
  if (!glfwInit()) {
    throw std::runtime_error("glfw init failed");
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // disable create opengl context
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);    // disable window resize
  window_ = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    throw std::runtime_error("Create window failed");
  }
  glfwSwapInterval(0);  // disable vsync
}

auto Window::GetRawWindow() -> GLFWwindow* {
  return window_;
}

auto Window::ShouldClose() -> bool {
  return glfwWindowShouldClose(window_);
}
auto Window::SwapBuffers() -> void {
  glfwPollEvents();
}

Window::~Window() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}
}  // namespace xac