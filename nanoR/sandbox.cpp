#include <memory>

#include "event/key_event.h"
#include "glad/glad.h"
#include "imgui.h"
#include "nanor.h"

class EditorLayer : public nanoR::Layer {
 public:
  EditorLayer(std::string name) : Layer(name) {
    scene_ = nanoR::GlobalContext::Instance().scene;
    main_camera_ = nanoR::GlobalContext::Instance().main_camera.get();
  }

  auto OnAttach() -> void override {
    auto cube_mesh_data = nanoR::Model("../resources/models/Cube/cube.obj");
    auto quad_mesh_data = nanoR::ResourceManager::GetQuadMeshData();
    auto* cube_mesh = nanoR::CreateMesh(&rhi_, cube_mesh_data.meshes_.at(0));
    quad_mesh_ = nanoR::CreateMesh(&rhi_, quad_mesh_data);

    cube_ = scene_->CreateEntity();
    auto cube_name = scene_->GetComponent<nanoR::NameComponent>(cube_);
    cube_name->name = "cube";
    auto c_mesh = cube_.AddComponent<nanoR::MeshComponent>();
    c_mesh->mesh = std::shared_ptr<nanoR::OpenGLMesh>(cube_mesh);

    auto quad = scene_->CreateEntity();
    c_mesh = quad.AddComponent<nanoR::MeshComponent>();
    c_mesh->mesh = std::shared_ptr<nanoR::OpenGLMesh>(quad_mesh_);
    // auto quad_name = scene_->GetComponent<nanoR::NameComponent>(quad);
    // quad_name->name = "quad";

    auto* point_light = new nanoR::Light{glm::vec3{1, 1, 1}, 100};
    auto light = scene_->CreateEntity();
    auto c_light = light.AddComponent<nanoR::LightCompoenent>();
    auto c_trans = light.GetComponenet<nanoR::TransformComponent>();
    c_trans->position = {6, -5, 4};
    c_light->light.reset(point_light);

    ui_shader_ = nanoR::GlobalContext::Instance().resource_manager->GetShader("ui");
    lit_shader_ = nanoR::GlobalContext::Instance().resource_manager->GetShader("lit");
    t_white_ = nanoR::ResourceManager::LoadTextureFromFile("../resources/textures/white.png");
    t_point_light_ = nanoR::ResourceManager::LoadTextureFromFile("../resources/textures/point-light.png");

    nanoR::RHIBufferCreateInfoOpenGL buffer_create_info;
    buffer_create_info.data = nullptr;
    buffer_create_info.size = 2 * sizeof(glm::mat4);
    buffer_create_info.flags = GL_DYNAMIC_STORAGE_BIT;
    rhi_.CreateBuffer(buffer_create_info, ubo_);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  auto Tick(uint64_t delta_time) -> void override {
    main_camera_->Tick(delta_time);
    auto* fbo = nanoR::GlobalContext::Instance().ui_layer->GetSceneFramebuffer();
    glBindFramebuffer(GL_FRAMEBUFFER, dynamic_cast<nanoR::RHIFramebufferOpenGL*>(fbo)->id);
    glClearColor(0.2, 0.2, 0.2, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto view = main_camera_->GetViewMatrix();
    auto proj = main_camera_->GetProjectionMatrix();
    glm::mat4 matrices[2]{view, proj};
    nanoR::RHISetBufferDataInfoOpenGL set_buffer_data_info;
    set_buffer_data_info.offset = 0;
    set_buffer_data_info.size = 2 * sizeof(glm::mat4);
    set_buffer_data_info.data = &matrices;
    rhi_.SetBufferData(set_buffer_data_info, ubo_.get());
    nanoR::RHIBindUniformBufferInfoOpenGL bind_uniform_buffer_info;
    bind_uniform_buffer_info.index = 0;
    bind_uniform_buffer_info.target = GL_UNIFORM_BUFFER;
    rhi_.BindUniformBuffer(bind_uniform_buffer_info, lit_shader_.get(), ubo_.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_white_);
    dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(lit_shader_.get())->SetValue<int>("texture_diffuse0", 0);
    for (auto&& [c_transform, c_mesh] : scene_->View<const nanoR::TransformComponent, const nanoR::MeshComponent>()) {
      dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(lit_shader_.get())->SetValue("model", c_transform.GetModelMatrix());
      rhi_.Draw(c_mesh.mesh->vao.get(), lit_shader_.get(), fbo);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_point_light_);
    for (auto&& [c_transform, c_light] : scene_->View<nanoR::TransformComponent, const nanoR::LightCompoenent>()) {
      c_transform.scale = glm::vec3{0.5};
      dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(ui_shader_.get())->SetValue("model", c_transform.GetModelMatrix());
      dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(ui_shader_.get())
          ->SetValue("ws_cam_pos", main_camera_->GetPosition());
      dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(ui_shader_.get())
          ->SetValue("color", glm::vec4{c_light.light->GetColor(), 1.0});
      dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(ui_shader_.get())->SetValue("tex", 0);
      dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(lit_shader_.get())
          ->SetValue("p_lights[0].color", c_light.light->GetColor());
      dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(lit_shader_.get())
          ->SetValue("p_lights[0].intensity", c_light.light->GetIntensity());
      dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(lit_shader_.get())
          ->SetValue("p_lights[0].ws_position", c_transform.position);
      rhi_.Draw(quad_mesh_->vao.get(), ui_shader_.get(), fbo);
    }
  }

  auto OnEvent(const std::shared_ptr<nanoR::Event>& event) -> bool override {
    return true;
  }

 private:
  std::shared_ptr<nanoR::RHIShaderProgram> lit_shader_;
  std::shared_ptr<nanoR::RHIShaderProgram> ui_shader_;
  std::shared_ptr<nanoR::Scene> scene_;
  nanoR::OpenGLMesh* quad_mesh_;
  GLuint t_point_light_ = 0;
  GLuint t_white_ = 0;
  std::shared_ptr<nanoR::RHIBuffer> ubo_;
  nanoR::RHIOpenGL rhi_;
  nanoR::PrespCamera* main_camera_;
  nanoR::Entity cube_;
};

class Sandbox : public nanoR::ApplicationOpenGL {
 public:
  Sandbox() = default;
};

auto main() -> int {
  std::unique_ptr<Sandbox> sandbox = std::make_unique<Sandbox>();
  auto editor_layer = std::make_shared<EditorLayer>("editor_layer");
  sandbox->PushLayer(editor_layer);
  LOG_TRACE("{}\n", sandbox->GetLayerStack().ToString());

  sandbox->Run();
  return 0;
}
