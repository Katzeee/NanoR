#pragma once
#include "rhi_type.h"

namespace nanoR {

class RHI {
 public:
  virtual ~RHI() = default;
  virtual auto CreateBuffer(const RHIBufferCreateInfo &buffer_create_info, std::shared_ptr<RHIBuffer> &buffer)
      -> bool = 0;
  virtual auto SetBufferData(const RHISetBufferDataInfo &set_buffer_data_info, RHIBuffer *buffer) -> bool = 0;
  virtual auto CreateVertexArray(std::shared_ptr<RHIVertexArray> &vertex_array) -> bool = 0;
  virtual auto BindVertexBuffer(
      const RHIBindVertexBufferInfo &bind_vertex_buffer_info, std::shared_ptr<RHIVertexArray> vertex_array,
      std::shared_ptr<RHIBuffer> vertex_buffer
  ) -> bool = 0;
  virtual auto BindIndexBuffer(
      const RHIBindIndexBufferInfo &bind_index_buffer_info, std::shared_ptr<RHIVertexArray> vertex_array,
      std::shared_ptr<RHIBuffer> index_buffer
  ) -> bool = 0;
  virtual auto CreateShaderModule(
      const RHIShaderModuleCreateInfo &shader_create_module_info, std::shared_ptr<RHIShaderModule> &shader_module
  ) -> bool = 0;
  virtual auto CreateShaderProgram(
      const RHIShaderProgramCreateInfo &shader_program_create_info, std::shared_ptr<RHIShaderProgram> &shader_program
  ) -> bool = 0;
  virtual auto SetShaderUniform(
      RHISetShaderUniformInfo const &set_shader_uniform_info, RHIShaderProgram *shader_program
  ) -> bool = 0;
  virtual auto BindUniformBuffer(const RHIBindUniformBufferInfo &bind_uniform_buffer_info, RHIBuffer *buffer)
      -> bool = 0;
  virtual auto CreateTexture(const RHITextureCreateInfo &texture_create_info, std::shared_ptr<RHITexture> &texture)
      -> bool = 0;
  virtual auto CreateFramebuffer(
      const RHIFramebufferCreateInfo &framebuffer_create_info, std::shared_ptr<RHIFramebuffer> &framebuffer
  ) -> bool = 0;
  virtual auto AttachTexture(
      const RHIAttachTextureInfo &attach_color_attachment_info, RHIFramebuffer const *framebuffer,
      RHITexture const *texture
  ) -> bool = 0;
  virtual bool Draw(
      RHIVertexArray const *vertex_array, RHIShaderProgram const *shader_program,
      std::optional<RHIFramebuffer const *> framebuffer
  ) = 0;
};

}  // namespace nanoR