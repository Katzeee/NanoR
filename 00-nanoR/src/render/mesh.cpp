#include "mesh.h"
#include "global/global_context.h"
#include "rhi.h"
#include <numeric>

namespace nanoR {

auto Mesh::GetVertexBuffer() -> std::shared_ptr<RHIBuffer> {
  const auto &rhi = GlobalContext::Instance().rhi;
  RHIBufferDesc desc{.usage = EBufferUsage::VERTEX};
  ResourceCreateInfo info{.data = nullptr};
  // caculate the stride of vertex data
  auto stride = GetAttributesStride();
  // the num of vertices * stride
  auto size = stride * std::get<std::vector<glm::vec3>>(attributes_[VertexAttributeType::POSITION]).size();
  info.data = malloc(size);
  uint64_t offset = 0;
  for (const auto &attr : attributes_) {
    switch (attr.first) {
    case VertexAttributeType::POSITION:
    case VertexAttributeType::NORMAL:
    case VertexAttributeType::TANGENT:
      FillVertexData<glm::vec3>(stride, offset, info.data, attr.second);
      break;
    case nanoR::VertexAttributeType::UV0:
    case nanoR::VertexAttributeType::UV1:
      FillVertexData<glm::vec2>(stride, offset, info.data, attr.second);
      break;
    }
  }
  return rhi->CreateBuffer(desc, info);
}

auto Mesh::GetIndexBuffer() -> std::shared_ptr<RHIBuffer> {
  const auto &rhi = GlobalContext::Instance().rhi;
  RHIBufferDesc desc{.usage = EBufferUsage::INDEX};
  ResourceCreateInfo info{.data = indices_.data()};
  return rhi->CreateBuffer(desc, info);
}

auto Mesh::GetAttributeTypeSize(VertexAttributeType type) -> uint64_t {
  switch (type) {
  case VertexAttributeType::POSITION:
  case VertexAttributeType::NORMAL:
  case VertexAttributeType::TANGENT:
    return sizeof(glm::vec3);
  case nanoR::VertexAttributeType::UV0:
  case nanoR::VertexAttributeType::UV1:
    return sizeof(glm::vec2);
  }
}

auto Mesh::GetAttributesStride() -> uint64_t {
  auto keys = std::views::keys(attributes_);
  return std::reduce(keys.begin(), keys.end(), 0, [](uint64_t value, auto &&attr) { return value + Mesh::GetAttributeTypeSize(attr); });
}
} // namespace nanoR