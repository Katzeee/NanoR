#include "material.h"

#include "global/global_context.h"
#include "platform/opengl/rhi_type_opengl.h"
#include "render/resource_manager.h"

namespace nanoR {
Material::Material() {
  shader_name_ = "lit";
  auto shader = GlobalContext::Instance().resource_manager->GetShader(shader_name_);
  uniform_descs = shader->ubo_descs;
}

auto Material::GetUniformDescs() -> std::map<std::string, UniformBufferDesc>& {
  return uniform_descs;
}

Material::Material(std::string_view shader_name) : shader_name_(shader_name) {}

auto Material::GetName() -> std::string_view {
  return shader_name_;
}

auto Material::PrepareUniforms(RHI* rhi) -> void {
  RHISetBufferDataInfoOpenGL set_buffer_data_info;
  for (auto const& ubo_desc : uniform_descs) {
    for (auto const& var : ubo_desc.second.vars) {
      std::visit(
          [&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            set_buffer_data_info.size = sizeof(T);
            set_buffer_data_info.data = &arg;
            set_buffer_data_info.offset = var.offset;
          },
          var.value
      );
      rhi->SetBufferData(set_buffer_data_info, ubo_desc.second.ubo.get());
    }
  }
}

}  // namespace nanoR