#include "light.h"

#include "global/global_context.h"
#include "resource/resource_manager.h"
#include "render/rhi.h"

namespace nanoR {

auto Light::GetColor() -> glm::vec3& {
  return color_;
}

auto Light::GetIntensity() -> float& {
  return intensity_;
}

struct DirectLight {
  float intensity;
  glm::vec3 direction;
  glm::vec3 color;
};

auto Light::GetUniforms() -> std::tuple<glm::vec3, float> {
  return std::make_tuple(color_, intensity_);
}

}  // namespace nanoR