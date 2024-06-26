#pragma once
#include <filesystem>
#include <shaderc/shaderc.hpp>

#include "render/rhi.h"
#include "resource/resource_manager.h"
#include "rhi_type_opengl.h"

#define OpenGLCheck(X)                               \
  X;                                                 \
  if (auto res = glGetError(); res != GL_NO_ERROR) { \
    LOG_FATAL("OpenGL API error {}\n", res);         \
    throw std::runtime_error("OpenGL API error");    \
  }

namespace nanoR {
class RHIOpenGL4 final : public RHI {
public:
  static RHIOpenGL4 *Get(RHI *rhi) {
    return dynamic_cast<RHIOpenGL4 *>(rhi);
  }
  ~RHIOpenGL4() override = default;
  static auto OpenGLCheckError() -> bool;
  auto CreateBuffer(const RHIBufferDesc &desc, const ResourceCreateInfo &info) -> std::shared_ptr<RHIBuffer> override;
  auto UpdateBufferData(const RHIUpdateBufferDataInfo &info, const std::shared_ptr<RHIBuffer> &buffer) -> void override;

  void DrawIndexed(const std::shared_ptr<RHIBuffer> &vertex_buffer, const std::shared_ptr<RHIBuffer> &index_buffer,
      const std::shared_ptr<RHIShaderProgram> shader_program, std::shared_ptr<RHIFramebuffer> framebuffer) override;

  // auto SetBufferData(const RHISetBufferDataInfo &set_buffer_data_info, RHIBuffer *buffer) -> bool override;
  // auto CreateVertexArray(std::shared_ptr<RHIVertexArray> &vertex_array) -> bool override;
  auto BindVertexBuffer(const RHIBindVertexBufferInfo &bind_vertex_buffer_info, std::shared_ptr<RHIVertexArrayOpenGL> vertex_array, std::shared_ptr<RHIBuffer> vertex_buffer) -> bool;
  auto BindIndexBuffer(const RHIBindIndexBufferInfo &bind_index_buffer_info, std::shared_ptr<RHIVertexArrayOpenGL> vertex_array, std::shared_ptr<RHIBuffer> index_buffer) -> bool;
  auto CreateShaderModule(const RHIShaderModuleCreateInfo &shader_module_create_info, std::shared_ptr<RHIShaderModule> &shader_module) -> bool override;
  auto CreateShaderProgram(const RHIShaderProgramCreateInfo &shader_program_create_info, std::shared_ptr<RHIShaderProgram> &shader_program) -> bool override;
  auto CreateTexture(const RHITextureCreateInfo &texture_create_info, std::shared_ptr<RHITexture> &texture) -> bool override;
  auto CreateFramebuffer(const RHIFramebufferCreateInfo &framebuffer_create_info, std::shared_ptr<RHIFramebuffer> &framebuffer) -> bool override;
  auto AttachTexture(const RHIAttachTextureInfo &attach_color_attachment_info, RHIFramebuffer const *framebuffer, RHITexture const *texture) -> bool override;

  // TODO: remove
  auto BindUniformBuffer(const RHIBindUniformBufferInfoOpenGL &bind_uniform_buffer_info, RHIBuffer *buffer) -> bool;
  // auto Draw(RHIVertexArray const *vertex_array, RHIShaderProgram const *shader_program, std::optional<RHIFramebuffer const *> framebuffer) -> bool override;
};

} // namespace nanoR