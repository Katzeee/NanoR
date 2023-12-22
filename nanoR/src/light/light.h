#pragma once
#include "nanorpch.h"

namespace nanoR {

class Light {
 public:
  Light(glm::vec3 color, float intensity) : color_(color), intensity_(intensity) {}
  auto GetColor() -> glm::vec3&;
  auto GetIntensity() -> float&;

 private:
  glm::vec3 color_;
  float intensity_;
};

class PointLight : public Light {
 public:
};
}  // namespace nanoR