#pragma once
#include <filesystem>
#include <shaderc/shaderc.hpp>

#include "render/resource_manager.h"
#include "render/rhi.h"
#include "rhi_type_opengl.h"

namespace nanoR {
class RHIOpenGL final : public RHI {
 public:
  ~RHIOpenGL() override = default;
  static auto OpenGLCheckError() -> bool;
  auto CreateBuffer(const RHIBufferCreateInfo &buffer_create_info, std::shared_ptr<RHIBuffer> &buffer) -> bool override;
  auto SetBufferData(const RHISetBufferDataInfo &set_buffer_data_info, RHIBuffer *buffer) -> bool override;
  auto CreateVertexArray(std::shared_ptr<RHIVertexArray> &vertex_array) -> bool override;
  auto BindVertexBuffer(
      const RHIBindVertexBufferInfo &bind_vertex_buffer_info, std::shared_ptr<RHIVertexArray> vertex_array,
      std::shared_ptr<RHIBuffer> vertex_buffer
  ) -> bool override;
  auto BindIndexBuffer(
      const RHIBindIndexBufferInfo &bind_index_buffer_info, std::shared_ptr<RHIVertexArray> vertex_array,
      std::shared_ptr<RHIBuffer> index_buffer
  ) -> bool override;
  auto CreateShaderModule(
      const RHIShaderModuleCreateInfo &shader_module_create_info, std::shared_ptr<RHIShaderModule> &shader_module
  ) -> bool override;
  auto CreateShaderProgram(
      const RHIShaderProgramCreateInfo &shader_program_create_info, std::shared_ptr<RHIShaderProgram> &shader_program
  ) -> bool override;
  auto SetShaderUniform(RHISetShaderUniformInfo const &set_shader_uniform_info, RHIShaderProgram *shader_program)
      -> bool override;
  auto BindUniformBuffer(RHIBindUniformBufferInfo const &bind_uniform_buffer_info, RHIBuffer *buffer) -> bool override;
  auto CreateTexture(const RHITextureCreateInfo &texture_create_info, std::shared_ptr<RHITexture> &texture)
      -> bool override;
  auto CreateFramebuffer(
      const RHIFramebufferCreateInfo &framebuffer_create_info, std::shared_ptr<RHIFramebuffer> &framebuffer
  ) -> bool override;
  auto AttachTexture(
      const RHIAttachTextureInfo &attach_color_attachment_info, RHIFramebuffer const *framebuffer,
      RHITexture const *texture
  ) -> bool override;
  auto Draw(
      RHIVertexArray const *vertex_array, RHIShaderProgram const *shader_program,
      std::optional<RHIFramebuffer const *> framebuffer
  ) -> bool override;

 private:
  auto ShaderToSpirv();
};

class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface {
 public:
  shaderc_include_result *GetInclude(
      const char *requested_source, shaderc_include_type type, const char *requesting_source, size_t include_depth
  ) override {
    LOG_TRACE("{}, {}, {}\n", requesting_source, requested_source, include_depth);
    std::filesystem::path p(requesting_source);
    p = p.parent_path() / requested_source;
    auto *content = new std::string(ResourceManager::ReadTextFromFile(p.c_str()));
    auto *res = new shaderc_include_result();
    res->user_data = content;
    res->content = content->c_str();
    res->content_length = content->length();
    // auto source_name = new std::string(requested_source);
    res->source_name = requested_source;
    return res;
  }

  void ReleaseInclude(shaderc_include_result *data) override {
    if (!data) return;
    delete static_cast<std::string *>(data->user_data);
    delete data;
  }

  virtual ~ShaderIncluder() = default;
};

}  // namespace nanoR