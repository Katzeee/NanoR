#pragma once
#include "nanorpch.h"

namespace nanoR {
struct MeshData {
  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal = {0, 0, 0};
    glm::vec2 texcoord = {0, 0};
    glm::vec3 tangent = {0, 0, 0};
  };

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
};
}  // namespace nanoR