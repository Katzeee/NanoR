#pragma once
#include "render/rhi_type.h"
#include <glm/glm.hpp>

namespace nanoR {

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec2 uv0;
  glm::vec2 uv1;
};

class SubMesh {
public:
  friend class AssetLoader;
  SubMesh() = default;
  SubMesh(const SubMesh &rhs);
  // using VertexAttributeData = std::variant<std::vector<glm::vec2>, std::vector<glm::vec3>>;
  auto GetVertexBuffer() -> std::shared_ptr<RHIBuffer>;
  auto GetIndexBuffer() -> std::shared_ptr<RHIBuffer>;
  constexpr auto GetAttributesStride() -> uint32_t;

private:
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
};

class Mesh {
public:
  friend class AssetLoader;
  auto GetSubMeshes() -> std::vector<SubMesh> &;

private:
  std::vector<SubMesh> submeshes_;
};
} // namespace nanoR