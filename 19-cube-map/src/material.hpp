#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace xac {
struct Texture {
  unsigned int id;
  std::string type;
};

struct Material {
  Material() = default;
  Material(const glm::vec3 &ka, const glm::vec3 &kd, const glm::vec3 &ks, std::vector<Texture> &&textures)
      : ka(ka), kd(kd), ks(ks), textures(std::move(textures)) {}
  glm::vec3 ka{0.2};
  glm::vec3 kd{1};
  glm::vec3 ks{1};
  std::vector<Texture> textures{};
};
};  // namespace xac
