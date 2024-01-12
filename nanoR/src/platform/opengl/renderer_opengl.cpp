#include "renderer_opengl.h"

#include "render/camera.h"
#include "render/rhi.h"
#include "scene/scene.hpp"

namespace nanoR {

RendererOpenGL::RendererOpenGL(RHI *rhi) {
  RHIBufferCreateInfoOpenGL buffer_create_info;
  RHIBindUniformBufferInfoOpenGL bind_uniform_buffer_info;
  bind_uniform_buffer_info.target = GL_UNIFORM_BUFFER;
  // create matrices ubo
  buffer_create_info.data = nullptr;
  buffer_create_info.size = 2 * sizeof(glm::mat4);
  buffer_create_info.flags = GL_DYNAMIC_STORAGE_BIT;
  rhi->CreateBuffer(buffer_create_info, ubo_matrices_);
  bind_uniform_buffer_info.index = 0;
  rhi->BindUniformBuffer(bind_uniform_buffer_info, ubo_matrices_.get());
  // create vectors ubo
  buffer_create_info.size = sizeof(glm::vec3);
  rhi->CreateBuffer(buffer_create_info, ubo_vectors_);
  bind_uniform_buffer_info.index = 1;
  rhi->BindUniformBuffer(bind_uniform_buffer_info, ubo_vectors_.get());
  // create per obj ubo
  buffer_create_info.size = sizeof(glm::mat4);
  rhi->CreateBuffer(buffer_create_info, ubo_per_objs_);
  bind_uniform_buffer_info.index = 2;
  bind_uniform_buffer_info.target = GL_UNIFORM_BUFFER;
  rhi->BindUniformBuffer(bind_uniform_buffer_info, ubo_per_objs_.get());
}

auto RendererOpenGL::PrepareUniforms(RHI *rhi, Camera *camera) -> void {
  // set matrices ubo
  auto view = camera->GetViewMatrix();
  auto proj = camera->GetProjectionMatrix();
  glm::mat4 matrices[2]{view, proj};
  RHISetBufferDataInfoOpenGL set_buffer_data_info;
  set_buffer_data_info.offset = 0;
  set_buffer_data_info.size = 2 * sizeof(glm::mat4);
  set_buffer_data_info.data = &matrices;
  rhi->SetBufferData(set_buffer_data_info, ubo_matrices_.get());
  // set camera ubo
  auto cam_pos = camera->GetPosition();
  set_buffer_data_info.size = 1 * sizeof(glm::vec3);
  set_buffer_data_info.data = &cam_pos;
  rhi->SetBufferData(set_buffer_data_info, ubo_vectors_.get());
}

auto RendererOpenGL::Render(RHI *rhi, Scene *scene, Camera *camera, RHIFramebuffer *framebuffer) -> void {
  PrepareUniforms(rhi, camera);
  RHISetBufferDataInfoOpenGL set_buffer_data_info;

  glBindFramebuffer(GL_FRAMEBUFFER, dynamic_cast<RHIFramebufferOpenGL *>(framebuffer)->id);
  glClearColor(0.2, 0.2, 0.2, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto &&[c_transform, c_mesh, c_mesh_renderer] :
       scene->View<const TransformComponent, const MeshComponent, const MeshRendererCompoenent>()) {
    // TODO: multiple materials
    auto shader = GlobalContext::Instance().resource_manager->GetShader(c_mesh_renderer.materials[0]->GetName());
    auto model = c_transform.GetModelMatrix();
    set_buffer_data_info.data = &model;
    set_buffer_data_info.offset = 0;
    set_buffer_data_info.size = sizeof(glm::mat4);
    rhi->SetBufferData(set_buffer_data_info, ubo_per_objs_.get());
    c_mesh_renderer.materials[0]->PrepareUniforms(rhi);
    rhi->Draw(c_mesh.mesh.get(), shader.get(), framebuffer);
  }

  // SECTION: Render lights
  auto ui_shader = GlobalContext::Instance().resource_manager->GetShader("ui");
  auto lit_shader = GlobalContext::Instance().resource_manager->GetShader("lit");
  auto point_light_tex = GlobalContext::Instance().resource_manager->GetTexture("point-light");
  // TODO: not get quad data here
  auto quad_data = GlobalContext::Instance().resource_manager->GetQuadMeshData();
  for (auto &&[c_transform, c_light] : scene->View<TransformComponent, const LightCompoenent>()) {
    c_transform.scale = glm::vec3{0.5};
    auto quad_mesh = CreateMesh(quad_data);
    auto model = c_transform.GetModelMatrix();
    set_buffer_data_info.data = &model;
    set_buffer_data_info.offset = 0;
    set_buffer_data_info.size = sizeof(glm::mat4);
    rhi->SetBufferData(set_buffer_data_info, ubo_per_objs_.get());

    c_light.light->PrepareUniforms(rhi, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dynamic_cast<RHITextureOpenGL *>(point_light_tex.get())->id);
    dynamic_cast<RHIShaderProgramOpenGL *>(ui_shader.get())->SetValue("albedo", 0);
    dynamic_cast<RHIShaderProgramOpenGL *>(lit_shader.get())->SetValue("p_lights[0].color", c_light.light->GetColor());
    dynamic_cast<RHIShaderProgramOpenGL *>(lit_shader.get())
        ->SetValue("p_lights[0].intensity", c_light.light->GetIntensity());
    dynamic_cast<RHIShaderProgramOpenGL *>(lit_shader.get())->SetValue("p_lights[0].ws_position", c_transform.position);
    rhi->Draw(quad_mesh.get(), ui_shader.get(), framebuffer);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace nanoR