#include "linux_window.h"

#include "event/key_event.h"
#include "event/mouse_event.h"
#include "event/window_event.h"
#include "nanorpch.h"

namespace nanoR {
LinuxWindow::LinuxWindow(WindowProp window_prop) {
  if (!window_) {
    Init(window_prop);
  }
}
auto LinuxWindow::Init(WindowProp window_prop) -> void {
  // SECTION: Create window
  if (!glfwInit()) {
    LOG_FATAL("GLFW initialize failed!\n");
    glfwTerminate();
    throw std::runtime_error("GLFW initialize failed!");
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHintString(GLFW_X11_CLASS_NAME, "opengl test");
  glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "opengl test");
  window_ = glfwCreateWindow(window_prop.width, window_prop.height, window_prop.title, nullptr, nullptr);
  if (!window_) {
    LOG_FATAL("Window initialize failed!\n");
    throw std::runtime_error("Window initialize failed!");
  }
  LOG_INFO("Window initialize with width {} height {} title \"{}\"\n", window_prop.width, window_prop.height,
           window_prop.title);
  glfwMakeContextCurrent(window_);
  glfwSetWindowUserPointer(window_, reinterpret_cast<void *>(&user_data_));

  // SECTION: Setup callbacks
  glfwSetWindowSizeCallback(window_, [](GLFWwindow *window, int width, int height) {
    UserData *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    auto window_resize_event = std::make_shared<WindowResizeEvent>(width, height);
    user_data->event_callback(window_resize_event);
  });
  glfwSetWindowCloseCallback(window_, [](GLFWwindow *window) {
    UserData *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    auto window_close_event = std::make_shared<WindowCloseEvent>();
    user_data->event_callback(window_close_event);
  });
  glfwSetCursorPosCallback(window_, [](GLFWwindow *window, double x, double y) {
    UserData *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    auto mouse_cursor_move_event = std::make_shared<MouseCursorMoveEvent>(x, y);
    user_data->event_callback(mouse_cursor_move_event);
  });
  glfwSetMouseButtonCallback(window_, [](GLFWwindow *window, int button, int action, int mods) {
    UserData *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
      auto mouse_button_down_event = std::make_shared<MouseButtonDownEvent>(button, mods);
      user_data->event_callback(mouse_button_down_event);
    } else if (action == GLFW_RELEASE) {
      auto mouse_button_up_event = std::make_shared<MouseButtonUpEvent>(button, mods);
      user_data->event_callback(mouse_button_up_event);
    }
  });
  glfwSetScrollCallback(window_, [](GLFWwindow *window, double xoffset, double yoffset) {
    UserData *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    auto mouse_button_scroll_event = std::make_shared<MouseButtonScrollEvent>(xoffset, yoffset);
    user_data->event_callback(mouse_button_scroll_event);
  });
  glfwSetKeyCallback(window_, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
    UserData *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
      auto key_down_event = std::make_shared<KeyDownEvent>(scancode, mods);
      user_data->event_callback(key_down_event);
    } else if (action == GLFW_RELEASE) {
      auto key_up_event = std::make_shared<KeyUpEvent>(scancode, mods);
      user_data->event_callback(key_up_event);
    }
  });
}

auto LinuxWindow::Tick() -> void {
  glfwSwapBuffers(window_);
  glfwPollEvents();
}

auto LinuxWindow::Shutdown() -> void {
  glfwDestroyWindow(window_);
  window_ = nullptr;
}

auto LinuxWindow::GetRawWindow() -> void * {
  return static_cast<void *>(window_);
}

LinuxWindow::~LinuxWindow() {
  Shutdown();
}

auto LinuxWindow::WindowResizeCallback(GLFWwindow *window, int width, int height) -> void {}
}  // namespace nanoR
