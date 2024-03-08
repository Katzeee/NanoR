#include "window_linux.h"

#include "event/key_event.h"
#include "event/mouse_event.h"
#include "event/window_event.h"
#include "nanorpch.h"

namespace nanoR {
WindowLinux::WindowLinux(WindowProp window_prop) {
  if (!window_) {
    Init(window_prop);
  }
}
auto WindowLinux::Init(WindowProp window_prop) -> void {
  // SECTION: Create window
  window_prop_ = window_prop;
  if (!glfwInit()) {
    LOG_FATAL("GLFW initialize failed!\n");
    glfwTerminate();
    throw std::runtime_error("GLFW initialize failed!");
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHintString(GLFW_X11_CLASS_NAME, "opengl test");
  glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "opengl test");
  window_ = glfwCreateWindow(window_prop.width, window_prop.height, window_prop.title, nullptr, nullptr);
  if (!window_) {
    LOG_FATAL("Window initialize failed!\n");
    throw std::runtime_error("Window initialize failed!");
  }
  LOG_TRACE(
      "Window initialize with width {} height {} title \"{}\"\n", window_prop.width, window_prop.height,
      window_prop.title
  );
  glfwMakeContextCurrent(window_);
  glfwSetWindowUserPointer(window_, reinterpret_cast<void *>(&user_data_));

  // SECTION: Setup callbacks
  glfwSetWindowSizeCallback(window_, [](GLFWwindow *window, int width, int height) {
    auto *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    auto window_resize_event = std::make_shared<WindowResizeEvent>(width, height);
    user_data->event_callback(window_resize_event);
  });
  glfwSetWindowCloseCallback(window_, [](GLFWwindow *window) {
    auto *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    auto window_close_event = std::make_shared<WindowCloseEvent>();
    user_data->event_callback(window_close_event);
  });
  glfwSetCursorPosCallback(window_, [](GLFWwindow *window, double x, double y) {
    auto *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    auto mouse_cursor_move_event = std::make_shared<MouseCursorMoveEvent>(x, y);
    user_data->event_callback(mouse_cursor_move_event);
  });
  glfwSetMouseButtonCallback(window_, [](GLFWwindow *window, int button, int action, int mods) {
    auto *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
      auto mouse_button_down_event = std::make_shared<MouseButtonDownEvent>(button, mods);
      user_data->event_callback(mouse_button_down_event);
    } else if (action == GLFW_RELEASE) {
      auto mouse_button_up_event = std::make_shared<MouseButtonUpEvent>(button, mods);
      user_data->event_callback(mouse_button_up_event);
    }
  });
  glfwSetScrollCallback(window_, [](GLFWwindow *window, double xoffset, double yoffset) {
    auto *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    auto mouse_button_scroll_event = std::make_shared<MouseButtonScrollEvent>(xoffset, yoffset);
    user_data->event_callback(mouse_button_scroll_event);
  });
  glfwSetKeyCallback(window_, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto *user_data = reinterpret_cast<UserData *>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
      // HINT: scancode is not related to the keyboard layout, like qwerty or colemak, dvorak
      auto key_down_event = std::make_shared<KeyDownEvent>(key, mods);
      user_data->event_callback(key_down_event);
    } else if (action == GLFW_RELEASE) {
      auto key_up_event = std::make_shared<KeyUpEvent>(key, mods);
      user_data->event_callback(key_up_event);
    }
  });
}

auto WindowLinux::Tick() -> void {
  glfwSwapBuffers(window_);
  glfwPollEvents();
}

auto WindowLinux::Shutdown() -> void {
  glfwDestroyWindow(window_);
  window_ = nullptr;
}

auto WindowLinux::GetRawWindow() -> void * {
  return static_cast<void *>(window_);
}

WindowLinux::~WindowLinux() {
  Shutdown();
}

auto WindowLinux::WindowResizeCallback(GLFWwindow *window, int width, int height) -> void {}
}  // namespace nanoR
