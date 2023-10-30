#include "application.h"

#include "global/global_context.h"

namespace nanoR {

Application::Application() {
  Init();
}

auto Application::Init() -> void {
  auto window = std::make_shared<Window>();
  window->user_data_.event_callback = std::bind(&Application::EventCallback, this, std::placeholders::_1);
  GlobalContext::Instance().window_ = window;
}

auto Application::Run() -> void {
  while (is_running_) {
    for (auto&& it : layer_stack_->GetLayers()) {
      it->Tick();
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