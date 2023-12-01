#include "camera.hpp"

#include <algorithm>

#include "context/context.hpp"
#include "input/input_system.hpp"

static glm::mat4 fitLightProjMatToCameraFrustum(
    glm::mat4 frustumMat, glm::vec4 lightDirection, float dim, bool square = false, bool roundToPixelSize = false,
    bool useConstantSize = false
) {
  // multiply by inverse projection*view matrix to find frustum vertices in world space
  // transform to light space
  // same pass, find minimum along each axis
  glm::mat4 lightSpaceTransform =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), -glm::vec3(lightDirection), glm::vec3(0.0f, 1.0f, 0.0f));

  bool firstProcessed = false;
  glm::vec3 boundingA(std::numeric_limits<float>::infinity());
  glm::vec3 boundingB(-std::numeric_limits<float>::infinity());

  // start with <-1 -1 -1> to <1 1 1> cube
  std::vector<glm::vec4> boundingVertices = {
      {-1.0f, -1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f, 1.0f},
      {1.0f, -1.0f, -1.0f, 1.0f},  {1.0f, -1.0f, 1.0f, 1.0f},  {1.0f, 1.0f, -1.0f, 1.0f},  {1.0f, 1.0f, 1.0f, 1.0f}};
  for (glm::vec4& vert : boundingVertices) {
    // clip space -> world space
    vert = frustumMat * vert;
    vert /= vert.w;
  }

  for (glm::vec4 vert : boundingVertices) {
    // clip space -> world space -> light space
    vert = lightSpaceTransform * vert;

    // initialize bounds without comparison, only for first transformed vertex
    if (!firstProcessed) {
      boundingA = glm::vec3(vert);
      boundingB = glm::vec3(vert);
      firstProcessed = true;
      continue;
    }

    // expand bounding box to encompass everything in 3D
    boundingA.x = std::min(vert.x, boundingA.x);
    boundingB.x = std::max(vert.x, boundingB.x);
    boundingA.y = std::min(vert.y, boundingA.y);
    boundingB.y = std::max(vert.y, boundingB.y);
    boundingA.z = std::min(vert.z, boundingA.z);
    boundingB.z = std::max(vert.z, boundingB.z);
  }

  // from https://en.wikipedia.org/wiki/Orthographic_projection#Geometry
  // because I don't trust GLM
  float l = boundingA.x;
  float r = boundingB.x;
  float b = boundingA.y;
  float t = boundingB.y;
  float n = boundingA.z;
  float f = boundingB.z;

  float actualSize;
  if (useConstantSize) {
    // keep constant world-size resolution, side length = diagonal of largest face of frustum
    // the other option looks good at high resolutions, but can result in shimmering as you look in different directions
    // and the cascade changes size
    float farFaceDiagonal = glm::length(glm::vec3(boundingVertices[7]) - glm::vec3(boundingVertices[1]));
    float forwardDiagonal = glm::length(glm::vec3(boundingVertices[7]) - glm::vec3(boundingVertices[0]));
    actualSize = std::max(farFaceDiagonal, forwardDiagonal);
  } else {
    actualSize = std::max(r - l, t - b);
  }

  // make it square
  if (square) {
    float W = r - l, H = t - b;
    float diff = actualSize - H;
    if (diff > 0) {
      t += diff / 2.0f;
      b -= diff / 2.0f;
    }
    diff = actualSize - W;
    if (diff > 0) {
      r += diff / 2.0f;
      l -= diff / 2.0f;
    }
  }

  // avoid shimmering
  if (roundToPixelSize) {
    float pixelSize = actualSize / dim;
    l = std::round(l / pixelSize) * pixelSize;
    r = std::round(r / pixelSize) * pixelSize;
    b = std::round(b / pixelSize) * pixelSize;
    t = std::round(t / pixelSize) * pixelSize;
  }

  glm::mat4 ortho = {
      2.0f / (r - l),
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      2.0f / (t - b),
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      2.0f / (f - n),
      0.0f,
      -(r + l) / (r - l),
      -(t + b) / (t - b),
      -(f + n) / (f - n),
      1.0f};
  ortho = glm::mat4{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, .5f, 0, 0, 0, .5f, 1} * ortho;

  // in world space -> light clip space
  ortho = ortho * lightSpaceTransform;

  return ortho;
}

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
    auto view = global_context.camera_->GetViewMatrix();
    auto proj = global_context.camera_->GetProjectionMatrix();
    auto pv_inverse = glm::inverse(proj * view);
    return fitLightProjMatToCameraFrustum(pv_inverse, glm::vec4{1, 1, 1, 0}, 1);
    // return glm::ortho(left_, right_, bottom_, top_, near_, far_);
  }
  throw std::logic_error("not implemented");
}

auto Camera::GetFrustumInWorld() -> std::array<glm::vec3, 8> {
  // clang-format off
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
  // clang-format on
  auto view = GetViewMatrix();
  auto proj = GetProjectionMatrix();
  auto pv_inverse = glm::inverse(proj * view);
  std::ranges::for_each(position, [&pv_inverse](auto&& p) {
    auto p_ws = pv_inverse * glm::vec4{p, 1.0};
    p = glm::vec3{p_ws / p_ws.w};
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