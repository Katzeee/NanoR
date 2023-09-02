#include "camera.hpp"
#include "context/context.hpp"
#include "input/input_system.hpp"

namespace xac {
glm::vec3 Camera::world_up_{0, 1, 0};
float Camera::sensitivity_ = 0.002;

void Camera::UpdateFreeCamera(float delta_time) {
  float speed = delta_time * 5.0f;
  if (InCommand(ControlCommand::FORWARD)) {
    PositionForward(front_ * speed);
  }
  if (InCommand(ControlCommand::BACKWARD)) {
    PositionForward(front_ * -speed);
  }
  if (InCommand(ControlCommand::RIGHT)) {
    PositionForward(glm::cross(front_, up_) * speed);
  }
  if (InCommand(ControlCommand::LEFT)) {
    PositionForward(glm::cross(front_, up_) * -speed);
  }
  if (InCommand(ControlCommand::UP)) {
    PositionForward(up_ * speed);
  }
  if (InCommand(ControlCommand::DOWN)) {
    PositionForward(up_ * -speed);
  }
}
void Camera::Tick(float delta_time) {
  UpdateCursorMove(InputSystem::cursor_x_offset_, InputSystem::cursor_y_offset_);
  UpdateScroll(InputSystem::scroll_y_offset_);
  UpdateFreeCamera(delta_time);
}
}  // end namespace xac