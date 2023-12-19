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
    auto* quad_mesh = nanoR::CreateMesh(&rhi_, quad_mesh_data);
    auto cube = scene_->CreateEntity();
    auto cube_name = scene_->Get<nanoR::NameComponent>(cube);
    cube_name->name = "cube";
    auto comp_sprite = cube.AddComponent<nanoR::MeshComponent>();
    comp_sprite->mesh = std::shared_ptr<nanoR::OpenGLMesh>(cube_mesh);

    // shader_program_ = nanoR::ResourceManager::GetUiShader(&rhi_);
    // shader_program_ = nanoR::ResourceManager::GetUnlitShader(&rhi_);
    shader_program_ = nanoR::ResourceManager::GetLitShader(&rhi_);
    // t_white_ = nanoR::ResourceManager::LoadTextureFromFile("../resources/textures/white.png");
    // auto t_point_light = nanoR::ResourceManager::LoadTextureFromFile("../resources/textures/point-light.png");
    t_white_ = nanoR::ResourceManager::LoadTextureFromFile("../resources/textures/point-light.png");

    nanoR::RHIBufferCreateInfoOpenGL buffer_create_info;
    buffer_create_info.data = nullptr;
    buffer_create_info.size = 2 * sizeof(glm::mat4);
    buffer_create_info.flags = GL_DYNAMIC_STORAGE_BIT;
    rhi_.CreateBuffer(buffer_create_info, ubo_);
    glEnable(GL_DEPTH_TEST);
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
    rhi_.BindUniformBuffer(bind_uniform_buffer_info, shader_program_.get(), ubo_.get());

    dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(shader_program_.get())->SetValue("model", glm::mat4{1});
    dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(shader_program_.get())->SetValue("view", view);
    dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(shader_program_.get())->SetValue("proj", proj);
    dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(shader_program_.get())
        ->SetValue("ws_cam_pos", main_camera_->GetPosition());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t_white_);
    dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(shader_program_.get())->SetValue<int>("texture_diffuse0", 0);

    auto v = scene_->View<nanoR::MeshComponent>();
    for (auto it = v.begin(); it != v.end(); ++it) {
      auto [mesh] = *it;
      rhi_.Draw(mesh.mesh->vao.get(), shader_program_.get(), fbo);
    }

    // rhi_.Draw(cube_mesh_.vao.get(), shader_program_.get(), fbo_.get());
    // rhi_.Draw(quad_mesh_.vao.get(), shader_program_.get(), fbo_.get());
  }

  auto TickUI() -> void override {
    ImGui::Begin("World");
    auto v = scene_->View<nanoR::NameComponent>();
    for (auto it = v.begin(); it != v.end(); ++it) {
      auto [name] = *it;
      ImGui::Text("%s", name.name.c_str());
    }
    ImGui::End();
    ImGui::ShowDemoWindow();
  }

  auto OnEvent(const std::shared_ptr<nanoR::Event>& event) -> bool override {
    return true;
  }

 private:
  std::shared_ptr<nanoR::RHIShaderProgram> shader_program_;
  std::shared_ptr<nanoR::Scene> scene_;
  GLuint t_white_ = 0;
  std::shared_ptr<nanoR::RHIBuffer> ubo_;
  nanoR::RHIOpenGL rhi_;
  nanoR::PrespCamera* main_camera_;
};

class InputLayer : public nanoR::Layer {
 public:
  InputLayer(std::string const& name) : Layer(name) {}
  auto OnEvent(std::shared_ptr<nanoR::Event> const& event) -> bool override {
    if (event->GetType() == nanoR::EventType::kKeyDown) {
      auto key_event = dynamic_cast<nanoR::KeyDownEvent*>(event.get());
    }
    return true;
  }
};

class Sandbox : public nanoR::ApplicationOpenGL {
 public:
  Sandbox() = default;
};

auto main() -> int {
  std::unique_ptr<Sandbox> sandbox = std::make_unique<Sandbox>();
  auto editor_layer = std::make_shared<EditorLayer>("editor_layer");
  auto input_layer = std::make_shared<InputLayer>("input layer");
  sandbox->PushLayer(editor_layer);
  sandbox->PushLayer(input_layer);
  LOG_TRACE("{}\n", sandbox->GetLayerStack().ToString());

  sandbox->Run();
  return 0;
}
