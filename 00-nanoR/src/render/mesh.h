#pragma once
#include "rhi_type.h"
#include <glm/glm.hpp>

namespace nanoR {

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv0;
  glm::vec2 uv1;
  glm::vec4 tangent;
};

enum class VertexAttributeType : uint8_t {
  POSITION,
  NORMAL,
  UV0,
  UV1,
  TANGENT,
};

// struct VertexAttribute {
//   VertexAttributeType type;
//   uint8_t size;
// };

class Mesh {
public:
  friend class AssetLoader;
  using VertexAttributeData = std::variant<std::vector<glm::vec2>, std::vector<glm::vec3>>;
  auto GetVertexBuffer() -> std::shared_ptr<RHIBuffer>;
  auto GetIndexBuffer() -> std::shared_ptr<RHIBuffer>;
  auto GetAttributesStride() -> uint64_t;

private:
  static auto GetAttributeTypeSize(VertexAttributeType type) -> uint64_t;

  template <typename T>
  auto FillVertexData(const uint64_t &stride, uint64_t &offset, void *dst, const VertexAttributeData &src) -> void {
    size_t i = 0;
    for (const auto &attr_value : std::get<std::vector<T>>(src)) {
      *reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(dst) + i * stride + offset) = attr_value;
      ++i;
    }
    offset += sizeof(T);
  }

private:
  std::map<VertexAttributeType, VertexAttributeData> attributes_;
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
};
} // namespace nanoR