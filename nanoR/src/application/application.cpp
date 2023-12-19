#include "application.h"

#include "global/global_context.h"
#include "layer/ui_layer.h"
#include "platform/input_system_glfw.h"
#include "render/camera.h"
#include "scene/scene.hpp"
#include "window/window.h"

namespace nanoR {

Application::Application() {
  Init();
}

auto Application::Init() -> void {
  auto window = std::make_shared<Window>();
  window->user_data_.event_callback = [this](auto&& event) { EventCallback(std::forward<decltype(event)>(event)); };
  GlobalContext::Instance().window = window;
  GlobalContext::Instance().input_system = std::make_shared<InputSystem<Platform::Linux>>();
  layer_stack_ = std::make_unique<LayerStack>();
  scene_ = std::make_shared<Scene>();
  GlobalContext::Instance().scene = scene_;
  GlobalContext::Instance().main_camera = std::make_shared<PrespCamera>(glm::vec3{0, 0, 5}, glm::vec3{0, 0, 0});
}

auto Application::Run() -> void {
  while (is_running_) {
    auto now = std::chrono::high_resolution_clock::now();
    static auto last = now;
    uint64_t delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
    last = now;
    for (auto&& it : layer_stack_->GetLayers()) {
      it->Tick(delta);
    }
    // FIX: execute order
    GlobalContext::Instance().input_system->Tick();
    GlobalContext::Instance().window->Tick();
  }
}

// HINT: use shared_ptr because the event is created by window
auto Application::EventCallback(std::shared_ptr<Event> const& event) -> void {
  // LOG_TRACE("{}\n", event->ToString());
  if (event->GetType() == EventType::kWindowClose) {
    is_running_ = false;
  }
  for (auto&& it : layer_stack_->GetLayers()) {
    if (!it->OnEvent(event)) {
      return;
    }
  }
  GlobalContext::Instance().input_system->OnEvent(event);
}

auto Application::PushLayer(std::shared_ptr<Layer> const& layer) -> void {
  layer_stack_->PushLayer(layer);
}
auto Application::PushOverlayLayer(std::shared_ptr<Layer> const& layer) -> void {
  layer_stack_->PushOverlayLayer(layer);
}
auto Application::PopLayer(std::shared_ptr<Layer> const& layer) -> void {
  layer_stack_->PopLayer(layer);
}

auto Application::GetLayerStack() -> LayerStack& {
  return *layer_stack_;
}

}  // namespace nanoR