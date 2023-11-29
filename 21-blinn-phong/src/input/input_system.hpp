#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <functional>
#include <iostream>
#include <vector>
// #include "context/context.hpp"

namespace xac {

enum class ControlCommand : uint32_t {
  FORWARD = 1 << 0,
  BACKWARD = 1 << 1,
  LEFT = 1 << 2,
  RIGHT = 1 << 3,
  UP = 1 << 4,
  DOWN = 1 << 5,
  SHOW_CURSOR = 1 << 6,
};

class InputSystem {
 public:
  // InputSystem() = delete;
  // ~InputSystem() = default;
  static void Init(GLFWwindow *window) {
    glfwSetScrollCallback(window, OnScroll);
    glfwSetCursorPosCallback(window, OnCursorPos);
    glfwSetKeyCallback(window, OnKey);
    glfwSetMouseButtonCallback(window, OnButton);
  }

  static void OnScroll(GLFWwindow *window, double x_offset, double y_offset) { scroll_y_offset_ = y_offset; }

  static void OnCursorPos(GLFWwindow *window, double x_pos, double y_pos) {
    static auto last_x_pos = static_cast<float>(x_pos);
    static auto last_y_pos = static_cast<float>(y_pos);
    // calculate the offset only when focus
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
      cursor_x_offset_ = x_pos - last_x_pos;
      cursor_y_offset_ = y_pos - last_y_pos;
    }
    // always record the last_pos or the view may flash
    last_x_pos = static_cast<float>(x_pos);
    last_y_pos = static_cast<float>(y_pos);
    // std::cout << x_pos << " " << y_pos << std::endl;
    // std::cout << cursor_x_offset_ << " " << cursor_y_offset_ << std::endl;
  }
  static void OnKey(GLFWwindow *window, int key, int scancode, int action, int mods);

  static void OnButton(GLFWwindow *window, int button, int action, int mods);

  static void Tick() {
    cursor_x_offset_ = 0;
    cursor_y_offset_ = 0;
    scroll_y_offset_ = 0;
  }

  static float cursor_x_offset_;
  static float cursor_y_offset_;
  static float scroll_y_offset_;
};

}  // end namespace xac