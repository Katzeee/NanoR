#include "material.h"

#include "global/global_context.h"
#include "platform/opengl/rhi_type_opengl.h"
#include "render/resource_manager.h"

namespace nanoR {
Material::Material() {
  shader_name_ = "lit";
  vec4_storage_["base_color"] = {1, 1, 1, 1};
  texture_storage_["albedo"] = GlobalContext::Instance().resource_manager->GetTexture("white");
}

Material::Material(std::string_view shader_name) : shader_name_(shader_name) {}

auto Material::SetVec4(std::string_view name, glm::vec4 const& value) -> void {
  vec4_storage_[name.data()] = value;
}

auto Material::GetName() -> std::string_view {
  return shader_name_;
}

auto Material::GetVec4(std::string_view name) -> glm::vec4& {
  if (auto it = vec4_storage_.find(name.data()); it != vec4_storage_.end()) {
    return it->second;
  }
  LOG_ERROR("No vec4 named {}\n", name);
  throw std::runtime_error("invalid vec4 name");
}

auto Material::SetTexture(std::string_view name, std::shared_ptr<RHITexture> const& texture) -> void {
  texture_storage_[name.data()] = texture;
}

auto Material::GetTexture(std::string_view name) -> RHITexture* {
  if (auto it = texture_storage_.find(name.data()); it != texture_storage_.end()) {
    return it->second.get();
  }
  LOG_ERROR("No texture named {}\n", name);
  throw std::runtime_error("invalid texture name");
}

auto Material::PrepareUniforms(RHI* rhi) -> void {
  auto shader = GlobalContext::Instance().resource_manager->GetShader(shader_name_);
  RHISetShaderUniformInfo set_shader_uniform_info;
  for (auto&& it : vec4_storage_) {
    set_shader_uniform_info.uniforms.emplace_back(it.first, it.second);
  }
  int i = 0;
  for (auto&& it : texture_storage_) {
    auto texture = dynamic_cast<RHITextureOpenGL*>(it.second.get());
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(texture->target, texture->id);
    set_shader_uniform_info.uniforms.emplace_back(it.first, i);
  }
  rhi->SetShaderUniform(set_shader_uniform_info, shader.get());
}

}  // namespace nanoR