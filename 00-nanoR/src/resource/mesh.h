#pragma once
#include "render/rhi_type.h"
#include <glm/glm.hpp>

namespace nanoR {

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv0;
  glm::vec2 uv1;
  glm::vec4 tangent;
};

class Mesh {
public:
  friend class AssetLoader;
  using VertexAttributeData = std::variant<std::vector<glm::vec2>, std::vector<glm::vec3>>;
  auto GetVertexBuffer() -> std::shared_ptr<RHIBuffer>;
  auto GetIndexBuffer() -> std::shared_ptr<RHIBuffer>;
  constexpr auto GetAttributesStride() -> uint32_t;

private:
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
  std::shared_ptr<Mesh> child_meshes_;
};
} // namespace nanoR