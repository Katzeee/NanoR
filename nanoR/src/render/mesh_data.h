#pragma once
#include "nanorpch.h"

namespace nanoR {
struct MeshData {
  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec3 tangent;
  };

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
};
}  // namespace nanoR