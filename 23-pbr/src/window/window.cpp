#include "window.hpp"

#include <stdexcept>

#include "camera/camera.hpp"
#include "context/context.hpp"

namespace xac {

class Camera;

Window::Window() {
  if (!glfwInit()) {
    throw std::runtime_error("glfw init failed");
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __linux__
  glfwWindowHintString(GLFW_X11_CLASS_NAME, "opengl test");
  glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "opengl test");
#endif

  window_ =
      glfwCreateWindow(global_context.window_width_, global_context.window_height_, "Hello OpenGL", nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    throw std::runtime_error("Create window failed");
  }
  glfwMakeContextCurrent(window_);
  glfwSwapInterval(0);  // disable vsync

  glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *window, int width, int height) {
    glViewport(global_context.imgui_width_, 0, width - global_context.imgui_width_, height);
    global_context.window_width_ = width;
    global_context.window_height_ = height;
    global_context.camera_->SetAspect(
        static_cast<float>(width - global_context.imgui_width_) / static_cast<float>(height)
    );
  });
  // glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

auto Window::GetRawWindow() -> GLFWwindow * {
  return window_;
}

auto Window::ShouldClose() -> bool {
  return glfwWindowShouldClose(window_);
}
auto Window::SwapBuffers() -> void {
  glfwSwapBuffers(window_);
  glfwPollEvents();
}

Window::~Window() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}
}  // namespace xac