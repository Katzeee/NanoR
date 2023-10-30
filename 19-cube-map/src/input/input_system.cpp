#include "input/input_system.hpp"

#include "context/context.hpp"
#include "imgui.h"

namespace xac {

float InputSystem::cursor_x_offset_ = 0;
float InputSystem::cursor_y_offset_ = 0;
float InputSystem::scroll_y_offset_ = 0;

void InputSystem::OnKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
      case GLFW_KEY_W:
        global_context.control_commad_ |= static_cast<uint32_t>(ControlCommand::FORWARD);
        break;
      case GLFW_KEY_A:
        global_context.control_commad_ |= static_cast<uint32_t>(ControlCommand::LEFT);
        break;
      case GLFW_KEY_S:
        global_context.control_commad_ |= static_cast<uint32_t>(ControlCommand::BACKWARD);
        break;
      case GLFW_KEY_D:
        global_context.control_commad_ |= static_cast<uint32_t>(ControlCommand::RIGHT);
        break;
      case GLFW_KEY_SPACE:
        global_context.control_commad_ |= static_cast<uint32_t>(ControlCommand::UP);
        break;
      case GLFW_KEY_LEFT_CONTROL:
        global_context.control_commad_ |= static_cast<uint32_t>(ControlCommand::DOWN);
        break;
      case GLFW_KEY_LEFT_ALT:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    }
  } else if (action == GLFW_RELEASE) {
    double cursor_x, cursor_y;
    int width, height;
    switch (key) {
      case GLFW_KEY_W:
        global_context.control_commad_ &= ~static_cast<uint32_t>(ControlCommand::FORWARD);
        break;
      case GLFW_KEY_A:
        global_context.control_commad_ &= ~static_cast<uint32_t>(ControlCommand::LEFT);
        break;
      case GLFW_KEY_S:
        global_context.control_commad_ &= ~static_cast<uint32_t>(ControlCommand::BACKWARD);
        break;
      case GLFW_KEY_D:
        global_context.control_commad_ &= ~static_cast<uint32_t>(ControlCommand::RIGHT);
        break;
      case GLFW_KEY_SPACE:
        global_context.control_commad_ &= ~static_cast<uint32_t>(ControlCommand::UP);
        break;
      case GLFW_KEY_LEFT_CONTROL:
        global_context.control_commad_ &= ~static_cast<uint32_t>(ControlCommand::DOWN);
        break;
      case GLFW_KEY_LEFT_ALT:
        // check whether cursor is inside the window
        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        glfwGetWindowSize(window, &width, &height);
        if (static_cast<int>(cursor_x) < width && static_cast<int>(cursor_y) < height && cursor_x > 0 && cursor_y > 0) {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        // std::cout << cursor_x << " " << cursor_y << std::endl;
        break;
    }
  }
}

void InputSystem::OnButton(GLFWwindow *window, int button, int action, int mods) {
  double cursor_x, cursor_y;
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse) {
    return;
  }
  if (action == GLFW_PRESS) {
  } else if (action == GLFW_RELEASE) {
    switch (button) {
      case GLFW_MOUSE_BUTTON_1:
        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        if (cursor_x > global_context.imgui_width_) {
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        break;
    }
  }
}
}  // end namespace xac