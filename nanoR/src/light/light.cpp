#include "light.h"

namespace nanoR {

auto Light::GetColor() -> glm::vec3& {
  return color_;
}

auto Light::GetIntensity() -> float& {
  return intensity_;
}

}  // namespace nanoR