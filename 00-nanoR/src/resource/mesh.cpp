#include "mesh.h"
#include "global/global_context.h"
#include "render/rhi.h"

namespace nanoR {

auto Mesh::GetVertexBuffer() -> std::shared_ptr<RHIBuffer> {
  const auto &rhi = GlobalContext::Instance().rhi;
  // caculate the stride of vertex data
  auto stride = GetAttributesStride();
  // the num of vertices * stride
  uint32_t size = stride * vertices_.size();
  RHIBufferDesc desc{.size = size, .usage = EBufferUsage::VERTEX};
  ResourceCreateInfo info{.data = vertices_.data()};
  return rhi->CreateBuffer(desc, info);
}

auto Mesh::GetIndexBuffer() -> std::shared_ptr<RHIBuffer> {
  const auto &rhi = GlobalContext::Instance().rhi;
  uint32_t size = sizeof(uint32_t) * indices_.size();
  RHIBufferDesc desc{.size = size, .usage = EBufferUsage::INDEX};
  ResourceCreateInfo info{.data = indices_.data()};
  return rhi->CreateBuffer(desc, info);
}

constexpr auto Mesh::GetAttributesStride() -> uint32_t {
  return sizeof(Vertex);
}
} // namespace nanoR