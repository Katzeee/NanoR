#pragma once
#include "event/event.h"
#include "event/key_event.h"
#include "event/mouse_event.h"
#include "global/global_context.h"
#include "input/input_system.h"
#include "window/window_base.h"

namespace nanoR {

auto ReceiveCommand(ControlCommand command) -> bool;

template <>
class InputSystem<Platform::Linux> {
 public:
  auto Tick() -> void {
    cursor_x_offset = 0;
    cursor_y_offset = 0;
    scroll_y_offset = 0;
  }

  auto OnEvent(std::shared_ptr<Event> const& event) {
    if ((event->GetCategory() & EventCategory::kInput) != EventCategory::kInput) {
      return;
    }
    if (event->GetType() == EventType::kKeyDown) {
      auto key_event = dynamic_cast<KeyDownEvent*>(event.get());
      switch (key_event->key_code) {
        case GLFW_KEY_W:
          control_commad |= static_cast<uint32_t>(ControlCommand::kForward);
          break;
        case GLFW_KEY_A:
          control_commad |= static_cast<uint32_t>(ControlCommand::kLeft);
          break;
        case GLFW_KEY_S:
          control_commad |= static_cast<uint32_t>(ControlCommand::kBackward);
          break;
        case GLFW_KEY_D:
          control_commad |= static_cast<uint32_t>(ControlCommand::kRight);
          break;
        default:
          break;
      }
    } else if (event->GetType() == EventType::kKeyUp) {
      auto key_event = dynamic_cast<KeyUpEvent*>(event.get());
      switch (key_event->key_code) {
        case GLFW_KEY_W:
          control_commad &= ~static_cast<uint32_t>(ControlCommand::kForward);
          break;
        case GLFW_KEY_A:
          control_commad &= ~static_cast<uint32_t>(ControlCommand::kLeft);
          break;
        case GLFW_KEY_S:
          control_commad &= ~static_cast<uint32_t>(ControlCommand::kBackward);
          break;
        case GLFW_KEY_D:
          control_commad &= ~static_cast<uint32_t>(ControlCommand::kRight);
          break;
        default:
          break;
      }
    } else if (event->GetType() == EventType::kMouseButtonDown) {
      auto button_event = dynamic_cast<MouseButtonDownEvent*>(event.get());
      glfwGetCursorPos(
          reinterpret_cast<GLFWwindow*>(GlobalContext::Instance().window->GetRawWindow()), &last_cursor_x_pos,
          &last_cursor_y_pos
      );
      switch (button_event->button_code) {
        case GLFW_MOUSE_BUTTON_1:
          control_commad |= static_cast<uint32_t>(ControlCommand::kLeftButtonDown);
          break;
        case GLFW_MOUSE_BUTTON_2:
          control_commad |= static_cast<uint32_t>(ControlCommand::kRightButtonDown);

          break;
        default:
          break;
      }
    } else if (event->GetType() == EventType::kMouseButtonUp) {
      auto button_event = dynamic_cast<MouseButtonUpEvent*>(event.get());
      switch (button_event->button_code) {
        case GLFW_MOUSE_BUTTON_1:
          control_commad &= ~static_cast<uint32_t>(ControlCommand::kLeftButtonDown);
          break;
        case GLFW_MOUSE_BUTTON_2:
          control_commad &= ~static_cast<uint32_t>(ControlCommand::kRightButtonDown);
          break;
        default:
          break;
      }
    } else if (event->GetType() == EventType::kMouseButtonScroll) {
      auto scroll_event = dynamic_cast<MouseButtonScrollEvent*>(event.get());
      scroll_y_offset = scroll_event->yoffset;
    }
    if (ReceiveCommand(ControlCommand::kLeftButtonDown) || ReceiveCommand(ControlCommand::kRightButtonDown)) {
      if (event->GetType() == EventType::kMouseCursorMove) {
        auto cursor_event = dynamic_cast<MouseCursorMoveEvent*>(event.get());
        cursor_x_offset = cursor_event->xpos - last_cursor_x_pos;
        cursor_y_offset = cursor_event->ypos - last_cursor_y_pos;
        last_cursor_x_pos = cursor_event->xpos;
        last_cursor_y_pos = cursor_event->ypos;
      }
    }
  }

  uint32_t control_commad = 0;
  double cursor_x_offset = 0;
  double cursor_y_offset = 0;
  double last_cursor_x_pos = 0;
  double last_cursor_y_pos = 0;
  double scroll_y_offset = 0;
};

inline static auto ReceiveCommand(ControlCommand command) -> bool {
  return static_cast<uint32_t>(command) & GlobalContext::Instance().input_system->control_commad;
}

}  // namespace nanoR