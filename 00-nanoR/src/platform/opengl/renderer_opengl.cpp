#include "renderer_opengl.h"

#include "render/camera.h"
#include "render/rhi.h"
#include "render/rhi_opengl4.h"
#include "render/rhi_type_opengl.h"
#include "scene/scene.hpp"

namespace nanoR {

RendererOpenGL::RendererOpenGL(RHI *rhi) {
  RHIBufferDesc buffer_desc = {
      .size = 3 * sizeof(glm::mat4) + sizeof(glm::vec3),
      .usage = EBufferUsage::UNIFORM};
  ResourceCreateInfo create_info = {.data = nullptr};

  RHIBindUniformBufferInfoOpenGL bind_uniform_buffer_info;
  bind_uniform_buffer_info.target = GL_UNIFORM_BUFFER;

  ubo_engine_ = rhi->CreateBuffer(buffer_desc, create_info);
  ubo_light_ = rhi->CreateBuffer(buffer_desc, create_info);

  bind_uniform_buffer_info.index = 0;
  RHIOpenGL4::Get(rhi)->BindUniformBuffer(bind_uniform_buffer_info, ubo_engine_.get());
  buffer_desc.size = 2 * sizeof(glm::vec3) + sizeof(float);
  bind_uniform_buffer_info.index = 1;
  RHIOpenGL4::Get(rhi)->BindUniformBuffer(bind_uniform_buffer_info, ubo_light_.get());
}

auto RendererOpenGL::PrepareUniforms(RHI *rhi, Camera *camera) -> void {
  // set matrices ubo
  auto view = camera->GetViewMatrix();
  auto proj = camera->GetProjectionMatrix();
  glm::mat4 matrices[2]{view, proj};
  RHIUpdateBufferDataInfo update_buffer_data_info;
  update_buffer_data_info.offset = 0;
  update_buffer_data_info.size = 2 * sizeof(glm::mat4);
  update_buffer_data_info.data = &matrices;
  rhi->UpdateBufferData(update_buffer_data_info, ubo_engine_);
  // set camera ubo
  auto cam_pos = camera->GetPosition();
  update_buffer_data_info.offset = 3 * sizeof(glm::mat4);
  update_buffer_data_info.size = 1 * sizeof(glm::vec3);
  update_buffer_data_info.data = &cam_pos;
  rhi->UpdateBufferData(update_buffer_data_info, ubo_engine_);
}

auto RendererOpenGL::Render(RHI *rhi, Scene *scene, Camera *camera, RHIFramebuffer *framebuffer) -> void {
  PrepareUniforms(rhi, camera);
  RHIUpdateBufferDataInfo update_buffer_data_info;

  glBindFramebuffer(GL_FRAMEBUFFER, dynamic_cast<RHIFramebufferOpenGL *>(framebuffer)->id);
  glClearColor(0.2, 0.2, 0.2, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto &&[c_transform, c_mesh, c_mesh_renderer] :
      scene->View<const TransformComponent, const MeshComponent, const MeshRendererCompoenent>()) {
    // TODO: multiple materials
    auto shader = GlobalContext::Instance().resource_manager->GetShader(c_mesh_renderer.materials[0]->GetName());
    auto model = c_transform.GetModelMatrix();
    update_buffer_data_info.data = &model;
    update_buffer_data_info.offset = 2 * sizeof(glm::mat4);
    update_buffer_data_info.size = sizeof(glm::mat4);
    RHIOpenGL4::Get(rhi)->UpdateBufferData(update_buffer_data_info, ubo_engine_);
    c_mesh_renderer.materials[0]->PrepareUniforms(rhi);
    // rhi->Draw(c_mesh.mesh.get(), shader.get(), framebuffer);
  }

  // SECTION: Render lights
  // auto ui_shader = GlobalContext::Instance().resource_manager->GetShader("ui");
  // auto lit_shader = GlobalContext::Instance().resource_manager->GetShader("lit");
  // auto point_light_tex = GlobalContext::Instance().resource_manager->GetTexture("point-light");
  // // TODO: not get quad data here
  // auto quad_data = GlobalContext::Instance().resource_manager->GetQuadMeshData();
  // for (auto &&[c_transform, c_light] : scene->View<TransformComponent, const LightCompoenent>()) {
  //   c_transform.scale = glm::vec3{0.5};
  //   auto quad_mesh = CreateMesh(quad_data);
  //   auto model = c_transform.GetModelMatrix();
  //   update_buffer_data_info.data = &model;
  //   update_buffer_data_info.offset = 2 * sizeof(glm::mat4);
  //   update_buffer_data_info.size = sizeof(glm::mat4);
  //   RHIOpenGL4::Get(rhi)->UpdateBufferData(update_buffer_data_info, ubo_engine_);

  //   auto light_data = c_light.light->GetUniforms();
  //   PointLight::PointLightType data = {std::get<0>(light_data), std::get<1>(light_data), c_transform.position};
  //   update_buffer_data_info.data = &data;
  //   update_buffer_data_info.offset = 0;
  //   update_buffer_data_info.size = sizeof(data);
  //   RHIOpenGL4::Get(rhi)->UpdateBufferData(update_buffer_data_info, ubo_light_);

  //   glActiveTexture(GL_TEXTURE0);
  //   glBindTexture(GL_TEXTURE_2D, dynamic_cast<RHITextureOpenGL *>(point_light_tex.get())->id);
  //   // dynamic_cast<RHIShaderProgramOpenGL *>(ui_shader.get())->SetValue("albedo", 0);
  //   // dynamic_cast<RHIShaderProgramOpenGL *>(lit_shader.get())->SetValue("p_lights[0].color",
  //   // c_light.light->GetColor()); dynamic_cast<RHIShaderProgramOpenGL *>(lit_shader.get())
  //   //     ->SetValue("p_lights[0].intensity", c_light.light->GetIntensity());
  //   // dynamic_cast<RHIShaderProgramOpenGL *>(lit_shader.get())->SetValue("p_lights[0].ws_position",
  //   // c_transform.position);
  //   rhi->Draw(quad_mesh.get(), ui_shader.get(), framebuffer);
  // }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace nanoR