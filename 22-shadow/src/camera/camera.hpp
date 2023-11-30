#pragma once
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

  Camera();
  Camera(glm::vec3 position, glm::vec3 target, ProjectionMethod projection_method = ProjectionMethod::PERSP);

  auto GetViewMatrix() -> glm::mat4 { return glm::lookAt(position_, position_ + front_, up_); }
  auto GetProjectionMatrix() -> glm::mat4;
  auto GetFrustumInWorld() -> std::array<glm::vec3, 8>;

  auto GetFornt() -> glm::vec3 { return front_; }
  auto GetUp() -> glm::vec3 { return up_; }
  auto GetPosition() -> glm::vec3 { return position_; }
  auto SetPosition(glm::vec3 position) { position_ = position; }
  auto SetAspect(float aspect) -> void { aspect_ = aspect; }
  auto SetFov(float fov) { fov_ = fov; }
  void SetOrtho(float left, float right, float bottom, float top);
  void SetNear(float near) { near_ = near; }
  void SetFar(float far) { far_ = far; }
  void UpdateFreeCamera(float delta_time);
  void Tick(float delta_time);
  void UpdateScroll(double y_offset);
  void UpdateCursorMove(float x_offset, float y_offset, float delta_time);
  
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

  // for perspective
  float fov_ = 45.0f;
  float aspect_ = 1.5f;
  float near_ = 0.1f;
  float far_ = 150.0f;
  // for ortho
  float left_ = -10.0f;
  float right_ = 10.0f;
  float bottom_ = -10.0f;
  float top_ = 10.0f;

  void PositionForward(glm::vec3 forward) { position_ += forward; }
  void CheckPitchSafety();
  void UpdateVectors();
};

}  // end namespace xac
