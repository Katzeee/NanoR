#pragma once
#include "render/rhi.h"
#include "rhi_type_opengl.h"

namespace nanoR {
class RHIOpenGL final : public RHI {
 public:
  ~RHIOpenGL() override = default;
  bool OpenGLCheckError();
  bool CreateBuffer(const RHIBufferCreateInfo &buffer_info, std::shared_ptr<RHIBuffer> &buffer) override;
  bool CreateVertexArray(std::shared_ptr<RHIVertexArray> &vertex_array) override;
  bool BindVertexBuffer(const RHIBindVertexBufferInfo &bind_vertex_buffer_info,
                        std::shared_ptr<RHIVertexArray> vertex_array, std::shared_ptr<RHIBuffer> buffer) override;
  bool BindIndexBuffer(const RHIBindIndexBufferInfo &bind_index_buffer_info,
                       std::shared_ptr<RHIVertexArray> vertex_array, std::shared_ptr<RHIBuffer> buffer) override;
  bool Draw(std::shared_ptr<RHIVertexArray> vertex_array) override;
};

}  // namespace nanoR