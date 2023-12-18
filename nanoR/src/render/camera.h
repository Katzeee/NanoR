#pragma once
#include "nanorpch.h"
#include "platform/input_system_glfw.h"

namespace nanoR {

enum class CameraType {
  kOrtho,
  kPersp,
};

template <CameraType T>
class Camera {};

template <>
class Camera<CameraType::kPersp> {
 public:
  Camera(glm::vec3 position, glm::vec3 target) : position_(position) {
    auto front = glm::normalize(target - position);
    pitch_ = std::asin(-front.y);
    yaw_ = atan2(-front.x, front.z);
    UpdateQuat();
  }
  auto GetPosition() -> glm::vec3 {
    return position_;
  }

  auto GetProjectionMatrix() -> glm::mat4 {
    return glm::perspective(fov_, aspect_, near_, far_);
  }

  auto GetViewMatrix() -> glm::mat4 {
    auto front = glm::rotate(rotation_, glm::vec3(0.0f, 0.0f, 1.0f));
    auto up = glm::rotate(rotation_, glm::vec3(0.0f, 1.0f, 0.0f));
    return glm::lookAt(position_, position_ + front, up);
  }

  // HINT: can't put input process to OnEvent because when frame count is low, event count will decrease
  auto Tick(uint64_t delta_time) -> void {
    float distance = delta_time * speed_ / 1000;
    auto front = glm::rotate(rotation_, glm::vec3(0.0f, 0.0f, 1.0f));
    auto up = glm::rotate(rotation_, glm::vec3(0.0f, 1.0f, 0.0f));
    auto right = glm::normalize(glm::cross(front, up));
    if (ReceiveCommand(ControlCommand::kForward)) {
      position_ += front * distance;
    }
    if (ReceiveCommand(ControlCommand::kBackward)) {
      position_ += front * -distance;
    }
    if (ReceiveCommand(ControlCommand::kRight)) {
      position_ += right * -distance;
    }
    if (ReceiveCommand(ControlCommand::kLeft)) {
      position_ += right * distance;
    }
    float cursor_x_offset = GlobalContext::Instance().input_system->cursor_x_offset;
    float cursor_y_offset = GlobalContext::Instance().input_system->cursor_y_offset;
    if (ReceiveCommand(ControlCommand::kLeftButtonDown)) {
      yaw_ += cursor_x_offset * translate_sensitivity_ * delta_time * fov_;
      pitch_ += cursor_y_offset * translate_sensitivity_ * delta_time * fov_;
    }
    if (ReceiveCommand(ControlCommand::kRightButtonDown)) {
      glm::vec3 rotate_origin = position_ + 5.0F * glm::normalize(front);
      auto rotate_matrix = glm::rotate(glm::mat4{1}, cursor_x_offset * rotate_sensitivity_, up);
      position_ = glm::vec3{rotate_matrix * glm::vec4{position_ - rotate_origin, 1.0}} + rotate_origin;
      rotate_matrix = glm::rotate(glm::mat4{1}, cursor_y_offset * rotate_sensitivity_, right);
      position_ = glm::vec3{rotate_matrix * glm::vec4{position_ - rotate_origin, 1.0}} + rotate_origin;
      front = glm::normalize(rotate_origin - position_);
      pitch_ = std::asin(-front.y);
      yaw_ = atan2(-front.x, front.z);
    }
    UpdateQuat();
  }

 private:
  void UpdateQuat() {
    rotation_ = glm::angleAxis(yaw_, glm::vec3(0.0f, -1.0f, 0.0f));
    rotation_ = glm::normalize(rotation_);
    // local right vector
    glm::vec3 right = glm::rotate(rotation_, glm::vec3(1.0f, 0.0f, 0.0f));
    // pitch calc is based on local right
    glm::quat rotQuat = glm::angleAxis(pitch_, right);
    rotation_ = rotQuat * rotation_;
    rotation_ = glm::normalize(rotation_);
  }

 public:
  glm::quat rotation_;
  // rotation around y, like shaking head
  float yaw_;
  // rotation around x
  float pitch_;
  glm::vec3 position_;
  inline static float speed_ = 1.5f;
  inline static float translate_sensitivity_ = 0.00002;
  inline static float rotate_sensitivity_ = 0.002;

  float fov_ = 43.0f;
  float aspect_ = 0.5f;
  float near_ = 0.1f;
  float far_ = 150.0f;
};

}  // namespace nanoR