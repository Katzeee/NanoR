#pragma once
#include "nanorpch.h"

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
   */
  auto PrepareUniforms(RHI *rhi, uint8_t number) -> void;

 private:
  glm::vec3 color_;
  float intensity_;
};

class PointLight : public Light {
 public:
};
}  // namespace nanoR