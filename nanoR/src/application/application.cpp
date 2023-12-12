#include "application.h"

#include "global/global_context.h"

namespace nanoR {

Application::Application() {
  Init();
}

auto Application::Init() -> void {
  auto window = std::make_shared<Window>();
  window->user_data_.event_callback = [this](auto&& event) { EventCallback(std::forward<decltype(event)>(event)); };
  GlobalContext::Instance().window_ = window;
}

auto Application::Run() -> void {
  while (is_running_) {
    auto now = std::chrono::high_resolution_clock::now();
    static auto last = now;
    uint64_t delta = std::chrono::duration_cast<std::chrono::microseconds>(now - last).count();
    last = now;
    for (auto&& it : layer_stack_->GetLayers()) {
      it->Tick(delta);
    }

    ui_layer_->Begin();
    for (auto&& it : layer_stack_->GetLayers()) {
      it->TickUI();
    }
    ui_layer_->End();
    GlobalContext::Instance().window_->Tick();
  }
}

auto Application::EventCallback(std::shared_ptr<Event> event) -> void {
  if (event->GetType() == EventType::kMouseCursorMove) {
    return;
  }
  LOG_TRACE("{}\n", event->ToString());
  if (event->GetType() == EventType::kWindowClose) {
    is_running_ = false;
  }
}

auto Application::PushLayer(std::shared_ptr<Layer> layer) -> void {
  layer_stack_->PushLayer(layer);
}
auto Application::PushOverlayLayer(std::shared_ptr<Layer> layer) -> void {
  layer_stack_->PushOverlayLayer(layer);
}
auto Application::PopLayer(std::shared_ptr<Layer> layer) -> void {
  layer_stack_->PopLayer(layer);
}

auto Application::GetLayerStack() -> LayerStack& {
  return *layer_stack_;
}

}  // namespace nanoR