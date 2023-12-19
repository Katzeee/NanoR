#include "application.h"

#include "global/global_context.h"
#include "platform/input_system_glfw.h"

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
  GlobalContext::Instance().ui_layer = std::make_shared<UILayer>("UILayer");
  PushOverlayLayer(GlobalContext::Instance().ui_layer);
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

    // ui_layer_->Begin();
    // for (auto&& it : layer_stack_->GetLayers()) {
    //   it->TickUI();
    // }
    // ui_layer_->End();

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