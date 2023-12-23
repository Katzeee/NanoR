#pragma once
#include "nanorpch.h"
#include "platform/input_system_glfw.h"

namespace nanoR {
class Camera {
 public:
  virtual auto GetProjectionMatrix() -> glm::mat4 = 0;
  virtual auto GetViewMatrix() -> glm::mat4 = 0;
};

class PrespCamera : public Camera {
 public:
  PrespCamera(glm::vec3 position, glm::vec3 target) : position_(position) {
    auto front = glm::normalize(target - position);
    pitch_ = std::asin(-front.y);
    yaw_ = atan2(-front.x, front.z);
    UpdateQuat();
  }
  auto GetPosition() -> glm::vec3 {
    return position_;
  }

  auto GetProjectionMatrix() -> glm::mat4 override {
    return glm::perspective(fov_, aspect_, near_, far_);
  }

  auto GetViewMatrix() -> glm::mat4 override {
    auto front = glm::rotate(rotation_, glm::vec3(0.0f, 0.0f, 1.0f));
    auto up = glm::rotate(rotation_, glm::vec3(0.0f, 1.0f, 0.0f));
    return glm::lookAt(position_, position_ + front, up);
  }

  // HINT: can't put input process to OnEvent because when frame count is low, event count will decrease
  auto Tick(uint64_t delta_time) -> void {
    float distance = speed_ * delta_time / 100;
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
    float scroll_y_offset = GlobalContext::Instance().input_system->scroll_y_offset;
    position_ += front * scroll_y_offset * distance * 10.0f;
    auto control_commad = GlobalContext::Instance().input_system->control_commad;
    switch (control_commad) {
      case static_cast<uint32_t>(ControlCommand::kLeftButtonDown):
        position_ += cursor_x_offset * distance * right * translate_sensitivity_;
        position_ += cursor_y_offset * distance * -up * translate_sensitivity_;
        break;
      case static_cast<uint32_t>(ControlCommand::kLeftButtonDown) | static_cast<uint32_t>(ControlCommand::kLeftAlt): {
        glm::vec3 rotate_origin = position_ + 5.0F * glm::normalize(front);
        auto rotate_matrix = glm::rotate(glm::mat4{1}, cursor_x_offset * rotate_sensitivity_ * delta_time, up);
        position_ = glm::vec3{rotate_matrix * glm::vec4{position_ - rotate_origin, 1.0}} + rotate_origin;
        rotate_matrix = glm::rotate(glm::mat4{1}, cursor_y_offset * rotate_sensitivity_ * delta_time, right);
        position_ = glm::vec3{rotate_matrix * glm::vec4{position_ - rotate_origin, 1.0}} + rotate_origin;
        front = glm::normalize(rotate_origin - position_);
        pitch_ = std::asin(-front.y);
        yaw_ = atan2(-front.x, front.z);
        break;
      }
      case static_cast<uint32_t>(ControlCommand::kRightButtonDown):
        yaw_ -= cursor_x_offset * rotate_sensitivity_ * delta_time * fov_ / 100;
        pitch_ -= cursor_y_offset * rotate_sensitivity_ * delta_time * fov_ / 100;
        break;
    }
    UpdateQuat();
  }

  auto SetAspect(float aspect) -> void {
    aspect_ = aspect;
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

 private:
  glm::quat rotation_;
  // rotation around y, like shaking head
  float yaw_;
  // rotation around x
  float pitch_;
  glm::vec3 position_;
  inline static float speed_ = 1.5f;
  inline static float translate_sensitivity_ = 0.15;
  inline static float rotate_sensitivity_ = 0.0002;

  float fov_ = 43.0f;
  float aspect_ = 0.5f;
  float near_ = 0.1f;
  float far_ = 1000.0f;
};

}  // namespace nanoR