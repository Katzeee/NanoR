#include "camera.hpp"
#include "context/context.hpp"
#include "input/input_system.hpp"
#include <algorithm>

namespace xac {
glm::vec3 Camera::world_up_{0, 1, 0};
float Camera::sensitivity_ = 0.02;

Camera::Camera() : Camera(glm::vec3{0, 0, 5}, glm::vec3{0}) {}

Camera::Camera(glm::vec3 position, glm::vec3 target, ProjectionMethod projection_method) : position_(position) {
  front_ = glm::normalize(target - position);
  pitch_ = std::asin(front_.y);
  yaw_ = (front_.z < 0 ? -1 : 1) * glm::acos(front_.x / std::cos(pitch_));
  UpdateVectors();
}

auto Camera::GetProjectionMatrix() -> glm::mat4 {
  if (projection_method_ == ProjectionMethod::PERSP) {
    return glm::perspective(fov_, aspect_, near_, far_);
  } else if (projection_method_ == ProjectionMethod::ORTHO) {
    return glm::ortho(left_, right_, bottom_, top_, near_, far_);
  }
  throw std::logic_error("not implemented");
}

auto Camera::GetFrustumInWorld() -> std::array<glm::vec3, 8> {
  std::array<glm::vec3, 8> position { // in ndc
    glm::vec3{-1.0f, -1.0f, -1.0f}, 
    glm::vec3{-1.0f, -1.0f,  1.0f}, 
    glm::vec3{-1.0f,  1.0f, -1.0f}, 
    glm::vec3{-1.0f,  1.0f,  1.0f}, 
    glm::vec3{ 1.0f,  1.0f, -1.0f}, 
    glm::vec3{ 1.0f,  1.0f,  1.0f}, 
    glm::vec3{ 1.0f, -1.0f, -1.0f}, 
    glm::vec3{ 1.0f, -1.0f,  1.0f}, 
  }; 
  auto view = GetViewMatrix();
  auto proj = GetProjectionMatrix();
  auto pv_inverse = glm::inverse(proj * view);
  std::ranges::for_each(position, [&pv_inverse](auto &&p){
    p = glm::vec3{pv_inverse * glm::vec4{p, 1.0}};
  });
  return position;
}

void Camera::SetOrtho(float left, float right, float bottom, float top) {
  left_ = left;
  right_ = right;
  bottom_ = bottom;
  top_ = top;
}

void Camera::UpdateScroll(double y_offset) {
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

void Camera::UpdateCursorMove(float x_offset, float y_offset, float delta_time) {
  // FIX: offset is not linear related to fov_
  yaw_ += x_offset * Camera::sensitivity_ * delta_time * fov_;
  pitch_ -= y_offset * Camera::sensitivity_ * delta_time * fov_;
  CheckPitchSafety();
  UpdateVectors();
}

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

void Camera::CheckPitchSafety() {
  if (pitch_ > 1.5533f) {
    pitch_ = 1.5533f;
  } else if (pitch_ < -1.5533f) {
    pitch_ = -1.5533f;
  }
}

void Camera::UpdateVectors() {
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

}  // end namespace xac