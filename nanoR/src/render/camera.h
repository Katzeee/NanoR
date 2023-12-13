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
    pitch_ = std::asin(front.y);
    yaw_ = atan2(-front.x, front.z);
    UpdateQuat();
  }
  auto GetProjectionMatrix() -> glm::mat4 {
    return glm::perspective(fov_, aspect_, near_, far_);
  }

  auto GetViewMatrix() -> glm::mat4 {
    auto front = glm::rotate(rotation_, glm::vec3(0.0f, 0.0f, 1.0f));
    auto up = glm::rotate(rotation_, glm::vec3(0.0f, 1.0f, 0.0f));
    return glm::lookAt(position_, position_ + front, up);
  }

  auto Tick(uint64_t delta_time) -> void {
    float distance = delta_time * speed_ / 1000;
    auto front = glm::rotate(rotation_, glm::vec3(0.0f, 0.0f, -1.0f));
    auto up = glm::rotate(rotation_, glm::vec3(0.0f, 1.0f, 0.0f));
    if (ReceiveCommand(ControlCommand::kForward)) {
      position_ += front * distance;
    }
    if (ReceiveCommand(ControlCommand::kBackward)) {
      position_ += front * -distance;
    }
    if (ReceiveCommand(ControlCommand::kRight)) {
      position_ += glm::cross(front, up) * -distance;
    }
    if (ReceiveCommand(ControlCommand::kLeft)) {
      position_ += glm::cross(front, up) * distance;
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

 private:
  glm::quat rotation_;
  // rotation around y, like shaking head
  float yaw_;
  // rotation around x
  float pitch_;
  glm::vec3 position_;
  float speed_ = 1.5f;

  float fov_ = 45.0f;
  float aspect_ = 1.5f;
  float near_ = 0.1f;
  float far_ = 150.0f;
};

}  // namespace nanoR