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
    auto cube1 = scene_->CreateCube();
    auto cube2 = scene_->CreateCube();
    auto p_light = scene_->CreatePointLight();
    // auto plane = scene_->CreateCube();
    // plane.GetComponenet<nanoR::NameComponent>()->name = "plane";
    // auto c_transform = plane.GetComponenet<nanoR::TransformComponent>();
    // c_transform->scale = {50, 0.001, 50};
    // c_transform->position = {0, -1, 0};
    // auto herta = scene_->LoadSampleScene();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  auto Tick(uint64_t delta_time) -> void override {
    main_camera_->Tick(delta_time);
    auto* fbo = nanoR::GlobalContext::Instance().ui_layer->GetSceneFramebuffer();
    nanoR::GlobalContext::Instance().renderer->Render(&rhi_, scene_.get(), main_camera_, fbo);
  }

  auto OnEvent(const std::shared_ptr<nanoR::Event>& event) -> bool override {
    return true;
  }

 private:
  std::shared_ptr<nanoR::Scene> scene_;
  nanoR::RHIOpenGL rhi_;
  nanoR::PrespCamera* main_camera_;
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
