#include "light.h"

#include "global/global_context.h"
#include "render/resource_manager.h"
#include "render/rhi.h"

namespace nanoR {

auto Light::GetColor() -> glm::vec3& {
  return color_;
}

auto Light::GetIntensity() -> float& {
  return intensity_;
}

auto Light::PrepareUniforms(RHI* rhi, uint8_t number) -> void {
  auto shader = GlobalContext::Instance().resource_manager->GetShader("ui");
  RHISetShaderUniformInfo set_shader_uniform_info;
  set_shader_uniform_info.uniforms.emplace_back("color", glm::vec4{color_, 1.0});
  rhi->SetShaderUniform(set_shader_uniform_info, shader.get());
}

}  // namespace nanoR