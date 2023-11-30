#include "camera.hpp"
#include "context/context.hpp"
#include "input/input_system.hpp"

namespace xac {
glm::vec3 Camera::world_up_{0, 1, 0};
float Camera::sensitivity_ = 0.02;

void Camera::UpdateFreeCamera(float delta_time) {
  float distance = delta_time * speed_;
  if (InCommand(ControlCommand::FORWARD)) {
    PositionForward(front_ * distance);
  }
  if (InCommand(ControlCommand::BACKWARD)) {
    PositionForward(front_ * -distance);
  }
  if (InCommand(ControlCommand::RIGHT)) {
    PositionForward(glm::cross(front_, up_) * distance);
  }
  if (InCommand(ControlCommand::LEFT)) {
    PositionForward(glm::cross(front_, up_) * -distance);
  }
  if (InCommand(ControlCommand::UP)) {
    PositionForward(world_up_ * distance);
  }
  if (InCommand(ControlCommand::DOWN)) {
    PositionForward(world_up_ * -distance);
  }
}
void Camera::Tick(float delta_time) {
  UpdateCursorMove(InputSystem::cursor_x_offset_, InputSystem::cursor_y_offset_, delta_time);
  UpdateScroll(InputSystem::scroll_y_offset_);
  UpdateFreeCamera(delta_time);
}
}  // end namespace xac