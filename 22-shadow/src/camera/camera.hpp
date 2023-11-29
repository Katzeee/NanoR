#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <cmath>
#include <exception>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

namespace xac {
class InputSystem;
enum class ControlCommand : uint32_t;

class Camera {
 public:
  enum class ProjectionMethod {
    ORTHO,
    PERSP,
    UNKONWN,
  };

  Camera() : Camera(glm::vec3{0, 0, 5}, glm::vec3{0}) {}

  Camera(glm::vec3 position, glm::vec3 target) : position_(position) {
    front_ = glm::normalize(target - position);
    pitch_ = std::asin(front_.y);
    yaw_ = (front_.z < 0 ? -1 : 1) * glm::acos(front_.x / std::cos(pitch_));
    UpdateVectors();
  }

  auto GetViewMatrix() -> glm::mat4 { return glm::lookAt(position_, position_ + front_, up_); }

  auto GetProjectionMatrix() -> glm::mat4 {
    if (projection_method_ == ProjectionMethod::PERSP) {
      return glm::perspective(fov_, aspect_, near_, far_);
    }
    throw std::logic_error("not implemented");
  }

  auto SetPosition(glm::vec3 position) { position_ = position; }
  auto GetFornt() -> glm::vec3 { return front_; }
  auto GetUp() -> glm::vec3 { return up_; }
  auto GetPosition() -> glm::vec3 { return position_; }
  auto SetAspect(float aspect) -> void { aspect_ = aspect; }
  auto SetFov(float fov) { fov_ = fov; }
  void UpdateFreeCamera(float delta_time);
  void Tick(float delta_time);
  void UpdateScroll(double y_offset) {
    if (fov_ >= 1.0f && fov_ <= 45.0f) {
      auto fov = fov_ - static_cast<float>(y_offset) * 0.05;
      SetFov(fov);
    }
    if (fov_ <= 1.0f) {
      SetFov(1.0f);
    }
    if (fov_ >= 45.0f) {
      SetFov(45.0f);
    }
  }
  void UpdateCursorMove(float x_offset, float y_offset) {
    yaw_ += x_offset * Camera::sensitivity_;
    pitch_ -= y_offset * Camera::sensitivity_;
    CheckPitchSafety();
    UpdateVectors();
  }
  auto GetYaw() -> float { return glm::degrees(yaw_); }
  auto GetPitch() -> float { return glm::degrees(pitch_); }
  void SetYaw(float yaw) {
    yaw_ = glm::radians(yaw);
    UpdateVectors();
  }
  void SetPitch(float pitch) {
    pitch_ = glm::radians(pitch);
    UpdateVectors();
  }

  float speed_ = 15.0f;

 private:
  glm::vec3 position_;
  glm::vec3 front_;
  glm::vec3 up_;
  float yaw_;
  float pitch_;

  static glm::vec3 world_up_;
  static float sensitivity_;

  ProjectionMethod projection_method_ = ProjectionMethod::PERSP;

  float fov_ = 45.0f;
  float aspect_ = 1.5f;
  float near_ = 0.1f;
  float far_ = 100.0f;

  void PositionForward(glm::vec3 forward) { position_ += forward; }

  void CheckPitchSafety() {
    if (pitch_ > 1.5533f) {
      pitch_ = 1.5533f;
    } else if (pitch_ < -1.5533f) {
      pitch_ = -1.5533f;
    }
  }

  void UpdateVectors() {
    front_.y = std::sin(pitch_);
    front_.x = std::cos(pitch_) * std::cos(yaw_);
    front_.z = std::cos(pitch_) * std::sin(yaw_);
    front_ = glm::normalize(front_);
    if (glm::distance(front_, glm::vec3{0, -1, 0}) < 0.01) {
      up_ = glm::vec3{0, 0, 1};
      return;
    } else if (glm::distance(front_, glm::vec3{0, 1, 0}) < 0.01) {
      up_ = glm::vec3{0, 0, -1};
      return;
    }
    glm::vec3 right = glm::normalize(glm::cross(front_, Camera::world_up_));
    up_ = glm::normalize(glm::cross(right, front_));
  };
};

}  // end namespace xac
