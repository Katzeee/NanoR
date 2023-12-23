#include "renderer_opengl.h"

#include "render/camera.h"
#include "render/rhi.h"
#include "scene/scene.hpp"

namespace nanoR {

auto Renderer::Render(RHI *rhi, Scene *scene, Camera *camera, RHIFramebuffer *framebuffer) -> void {
  // create ubo
  std::shared_ptr<nanoR::RHIBuffer> ubo_;
  nanoR::RHIBufferCreateInfoOpenGL buffer_create_info;
  buffer_create_info.data = nullptr;
  buffer_create_info.size = 2 * sizeof(glm::mat4);
  buffer_create_info.flags = GL_DYNAMIC_STORAGE_BIT;
  rhi->CreateBuffer(buffer_create_info, ubo_);

  // render
  glBindFramebuffer(GL_FRAMEBUFFER, dynamic_cast<nanoR::RHIFramebufferOpenGL *>(framebuffer)->id);
  glClearColor(0.2, 0.2, 0.2, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto view = camera->GetViewMatrix();
  auto proj = camera->GetProjectionMatrix();
  glm::mat4 matrices[2]{view, proj};
  nanoR::RHISetBufferDataInfoOpenGL set_buffer_data_info;
  set_buffer_data_info.offset = 0;
  set_buffer_data_info.size = 2 * sizeof(glm::mat4);
  set_buffer_data_info.data = &matrices;
  rhi->SetBufferData(set_buffer_data_info, ubo_.get());
  nanoR::RHIBindUniformBufferInfoOpenGL bind_uniform_buffer_info;
  bind_uniform_buffer_info.index = 0;
  bind_uniform_buffer_info.target = GL_UNIFORM_BUFFER;

  rhi->BindUniformBuffer(bind_uniform_buffer_info, ubo_.get());
  for (auto &&[c_transform, c_mesh, c_mesh_renderer] :
       scene->View<const nanoR::TransformComponent, const nanoR::MeshComponent, const nanoR::MeshRendererCompoenent>(
       )) {
    auto shader = nanoR::GlobalContext::Instance().resource_manager->GetShader(c_mesh_renderer.materials[0]->GetName());
    dynamic_cast<nanoR::RHIShaderProgramOpenGL *>(shader.get())->SetValue("model", c_transform.GetModelMatrix());
    c_mesh_renderer.materials[0]->PrepareUniforms(rhi);
    rhi->Draw(c_mesh.mesh.get(), shader.get(), framebuffer);
  }

  // for (auto &&[c_transform, c_light] : scene->View<nanoR::TransformComponent, const nanoR::LightCompoenent>()) {
  //   c_transform.scale = glm::vec3{0.5};
  //   dynamic_cast<nanoR::RHIShaderProgramOpenGL *>(ui_shader_.get())->SetValue("model", c_transform.GetModelMatrix());
  //   dynamic_cast<nanoR::RHIShaderProgramOpenGL *>(ui_shader_.get())
  //       ->SetValue("ws_cam_pos", main_camera_->GetPosition());
  //   dynamic_cast<nanoR::RHIShaderProgramOpenGL *>(ui_shader_.get())
  //       ->SetValue("color", glm::vec4{c_light.light->GetColor(), 1.0});
  //   dynamic_cast<nanoR::RHIShaderProgramOpenGL *>(ui_shader_.get())->SetValue("tex", 0);
  //   dynamic_cast<nanoR::RHIShaderProgramOpenGL *>(lit_shader_.get())
  //       ->SetValue("p_lights[0].color", c_light.light->GetColor());
  //   dynamic_cast<nanoR::RHIShaderProgramOpenGL *>(lit_shader_.get())
  //       ->SetValue("p_lights[0].intensity", c_light.light->GetIntensity());
  //   dynamic_cast<nanoR::RHIShaderProgramOpenGL *>(lit_shader_.get())
  //       ->SetValue("p_lights[0].ws_position", c_transform.position);
  //   rhi_.Draw(quad_mesh_->vao.get(), ui_shader_.get(), fbo);
  // }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace nanoR