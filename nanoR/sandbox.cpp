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
    auto cube = scene_->CreateCube();

    auto* point_light = new nanoR::Light{glm::vec3{1, 1, 1}, 100};
    auto light = scene_->CreateEntity();
    auto c_light = light.AddComponent<nanoR::LightCompoenent>();
    auto c_trans = light.GetComponenet<nanoR::TransformComponent>();
    c_trans->position = {6, -5, 4};
    c_light->light.reset(point_light);

    ui_shader_ = nanoR::GlobalContext::Instance().resource_manager->GetShader("ui");
    lit_shader_ = nanoR::GlobalContext::Instance().resource_manager->GetShader("lit");

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
    nanoR::Renderer::Render(&rhi_, scene_.get(), main_camera_, fbo);

    // dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(lit_shader_.get())->SetValue<int>("albedo", 0);
    // for (auto&& [c_transform, c_mesh, c_mesh_renderer] :
    //      scene_->View<const nanoR::TransformComponent, const nanoR::MeshComponent, const
    //      nanoR::MeshRendererCompoenent>(
    //      )) {
    //   auto shader =
    //       nanoR::GlobalContext::Instance().resource_manager->GetShader(c_mesh_renderer.materials[0]->GetName());
    //   dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(shader.get())->SetValue("model", c_transform.GetModelMatrix());
    //   c_mesh_renderer.materials[0]->PrepareUniforms(&rhi_);
    //   rhi_.Draw(c_mesh.mesh->vao.get(), lit_shader_.get(), fbo);
    // }
  }

  auto OnEvent(const std::shared_ptr<nanoR::Event>& event) -> bool override {
    return true;
  }

 private:
  std::shared_ptr<nanoR::RHIShaderProgram> lit_shader_;
  std::shared_ptr<nanoR::RHIShaderProgram> ui_shader_;
  std::shared_ptr<nanoR::Scene> scene_;
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
