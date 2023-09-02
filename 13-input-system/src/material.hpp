#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace xac {
struct Texture {
  unsigned int id_;
  std::string type_;
};

struct Material {
  Material() = default;
  Material(glm::vec3 &&Ka, glm::vec3 &&Kd, glm::vec3 &&Ks, std::vector<Texture> &&textures)
      : Ka_(Ka), Kd_(Kd), Ks_(Ks), textures_(textures) {}
  glm::vec3 Ka_{1};
  glm::vec3 Kd_{1};
  glm::vec3 Ks_{1};
  std::vector<Texture> textures_{};
};
};  // namespace xac
