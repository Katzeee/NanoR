#pragma once
#include <GLFW/glfw3.h>
#include <exception>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace xac {
class Camera {
public:
  enum class ProjectionMethod {
    ORTHO,
    PERSP,
  };

  Camera() = delete;
  Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up);
  auto GetViewMatrix() -> glm::mat4;
  auto GetProjectionMatrix() -> glm::mat4;

  void UpdateScroll(double x_offset);
  auto GetFornt() -> glm::vec3;
  void SetFront(glm::vec3 front);
  void PositionForward(glm::vec3 forward);
  auto GetUp() -> glm::vec3;
  auto GetPosition() -> glm::vec3;

private:
  glm::vec3 position_;
  glm::vec3 front_;
  glm::vec3 up_;

  ProjectionMethod projection_method_ = ProjectionMethod::PERSP;

  float fov_ = 45.0f;
  float aspect_ = 1.0f;
  float near_ = 0.1f;
  float far_ = 100.0f;
};

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up)
    : position_(position), front_(front), up_(up) {}

auto Camera::GetViewMatrix() -> glm::mat4 {
  return glm::lookAt(position_, position_ + front_, up_);
}
auto Camera::GetProjectionMatrix() -> glm::mat4 {
  if (projection_method_ == ProjectionMethod::PERSP) {
    return glm::perspective(fov_, aspect_, near_, far_);
  }
  throw std::logic_error("not implemented");
}

void Camera::UpdateScroll(double y_offset) {
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

auto Camera::GetFornt() -> glm::vec3 { return front_; }

void Camera::SetFront(glm::vec3 front) { front_ = front; }

auto Camera::GetUp() -> glm::vec3 { return up_; }

auto Camera::GetPosition() -> glm::vec3 { return position_; }

void Camera::PositionForward(glm::vec3 forward) { position_ += forward; }

} // end namespace xac