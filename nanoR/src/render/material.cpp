#include "material.h"

#include "global/global_context.h"
#include "platform/opengl/rhi_type_opengl.h"
#include "render/resource_manager.h"

namespace nanoR {
Material::Material() {
  shader_name_ = "lit";
  auto shader = GlobalContext::Instance().resource_manager->GetShader(shader_name_);
  uniforms = shader->ubo_descs;
  // for (auto uniform : uniforms) {
  //   std::visit(
  //       [&](auto&& arg) {
  //         using T = std::decay_t<decltype(arg)>;
  //         // auto* arg_p = std::get_if<T>(&uniform.second.value);
  //         if constexpr (std::is_same_v<T, int>) {
  //         } else if constexpr (std::is_same_v<T, float>) {
  //         } else if constexpr (std::is_same_v<T, glm::vec3>) {
  //           LOG_TRACE("x: {}, y: {}, z: {}\n", arg.x, arg.y, arg.z);
  //           arg = {0, 0, 1};
  //           LOG_TRACE("x: {}, y: {}, z: {}\n", arg.x, arg.y, arg.z);
  //         } else if constexpr (std::is_same_v<T, glm::vec4>) {
  //           LOG_TRACE("x: {}, y: {}, z: {}, a: {}\n", arg.x, arg.y, arg.z, arg.a);
  //           arg = {1, 0, 0, 1};
  //           LOG_TRACE("x: {}, y: {}, z: {}, a: {}\n", arg.x, arg.y, arg.z, arg.a);
  //         }
  //       },
  //       uniform.second.value
  //   );
  // }

  // vec4_storage_["base_color"] = {1, 1, 1, 1};
  // texture_storage_["albedo"] = GlobalContext::Instance().resource_manager->GetTexture("white");
}

auto Material::GetUniforms() -> std::map<std::string, UniformBufferDesc>& {
  return uniforms;
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
  RHISetBufferDataInfoOpenGL set_buffer_data_info;
  for (auto const& ubo_desc : uniforms) {
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