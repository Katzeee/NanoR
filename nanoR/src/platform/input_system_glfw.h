#pragma once
#include "event/event.h"
#include "event/key_event.h"
#include "input/input_system.h"

namespace nanoR {

template <>
class InputSystem<Platform::Linux> {
 public:
  auto OnEvnet(std::shared_ptr<Event> const& event) {
    if ((event->GetCategory() & EventCategory::kKey) != EventCategory::kKey) {
      return;
    }
    if (event->GetType() == EventType::kKeyDown) {
      auto key_event = dynamic_cast<KeyDownEvent*>(event.get());
      switch (key_event->key_code) {
        case GLFW_KEY_W:
          control_commad |= static_cast<uint32_t>(ControlCommand::FORWARD);
          break;
        case GLFW_KEY_A:
          control_commad |= static_cast<uint32_t>(ControlCommand::LEFT);
          break;
        case GLFW_KEY_S:
          control_commad |= static_cast<uint32_t>(ControlCommand::BACKWARD);
          break;
        case GLFW_KEY_D:
          control_commad |= static_cast<uint32_t>(ControlCommand::RIGHT);
          break;
        default:
          break;
      }
    } else if (event->GetType() == EventType::kKeyUp) {
      auto key_event = dynamic_cast<KeyUpEvent*>(event.get());
      switch (key_event->key_code) {
        case GLFW_KEY_W:
          control_commad &= ~static_cast<uint32_t>(ControlCommand::FORWARD);
          break;
        case GLFW_KEY_A:
          control_commad &= ~static_cast<uint32_t>(ControlCommand::LEFT);
          break;
        case GLFW_KEY_S:
          control_commad &= ~static_cast<uint32_t>(ControlCommand::BACKWARD);
          break;
        case GLFW_KEY_D:
          control_commad &= ~static_cast<uint32_t>(ControlCommand::RIGHT);
          break;
        default:
          break;
      }
    }
  }

  inline static uint32_t control_commad = 0;
};

}  // namespace nanoR