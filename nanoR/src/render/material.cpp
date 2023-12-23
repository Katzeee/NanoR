#include "material.h"

#include "global/global_context.h"
#include "render/resource_manager.h"
#include "render/rhi.h"

namespace nanoR {
Material::Material() {
  shader_name_ = "lit";
  vec4_storage_["base_color"] = {1, 1, 1, 1};
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

auto Material::PrepareUniforms(RHI* rhi) -> void {
  auto shader = GlobalContext::Instance().resource_manager->GetShader(shader_name_);
  RHISetShaderUniformInfo set_shader_uniform_info;
  for (auto&& it : vec4_storage_) {
    set_shader_uniform_info.uniforms.emplace_back(it.first, it.second);
  }
  rhi->SetShaderUniform(set_shader_uniform_info, shader.get());
}

}  // namespace nanoR