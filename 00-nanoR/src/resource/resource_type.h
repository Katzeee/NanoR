#pragma once
#include "nanorpch.h"

namespace nanoR {

enum class RenderApi {
  kOpenGL,
  kVulkan,
};

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

struct ShaderData {
  std::string vs_src;
  std::string fs_src;
  std::map<std::string, std::set<std::string>> defined_symbols_;
};

}  // namespace nanoR