#pragma once
#include <GLFW/glfw3.h>
#include <cmath>
#include <exception>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

namespace xac {
class Camera {
 public:
  enum class ProjectionMethod {
    ORTHO,
    PERSP,
    UNKONWN,
  };

  Camera() = delete;

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

  void UpdateScroll(double y_offset) {
    if (fov_ >= 1.0f && fov_ <= 45.0f) {
      fov_ -= static_cast<float>(y_offset) * 0.05;
    }
    if (fov_ <= 1.0f) {
      fov_ = 1.0f;
    }
    if (fov_ >= 45.0f) {
      fov_ = 45.0f;
    }
  }

  void UpdateCursorMove(float x_offset, float y_offset) {
    yaw_ += x_offset * Camera::sensitivity_;
    pitch_ -= y_offset * Camera::sensitivity_;
    CheckPitchSafety();
    UpdateVectors();
  }

  auto GetFornt() -> glm::vec3 { return front_; }

  void PositionForward(glm::vec3 forward) { position_ += forward; }

  auto GetUp() -> glm::vec3 { return up_; }

  auto GetPosition() -> glm::vec3 { return position_; }

  auto SetAspect(float aspect) -> void { aspect_ = aspect; }

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
    glm::vec3 right = glm::normalize(glm::cross(front_, Camera::world_up_));
    up_ = glm::normalize(glm::cross(right, front_));
  };
};

glm::vec3 Camera::world_up_{0, 1, 0};
float Camera::sensitivity_ = 0.002;

}  // end namespace xac
