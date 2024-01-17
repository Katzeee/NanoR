#pragma once
#include "nanorpch.h"
#include "platform/opengl/rhi_type_opengl.h"

namespace nanoR {

class RHI;

class Light {
 public:
  Light(glm::vec3 color, float intensity) : color_(color), intensity_(intensity) {}
  auto GetColor() -> glm::vec3 &;
  auto GetIntensity() -> float &;
  /**
   * @brief prepare the uniforms
   *
   * @param rhi rhi
   * @param number the number of this light
   * @param ubo_light light ubo
   */
  auto GetUniforms() -> std::tuple<glm::vec3, float>;

 private:
  glm::vec3 color_;
  float intensity_;
};

class PointLight : public Light {
 public:
  struct PointLightType {
    glm::vec3 color;
    float intensity;
    glm::vec3 ws_position;
  };
};

class DirectionLight : public Light {};
}  // namespace nanoR