#include "application.h"

#include "window/linux_window.h"

namespace nanoR {

Application::Application() {
  Init();
}

auto Application::Init() -> void {
  window_ = std::make_unique<LinuxWindow>();
  window_->user_data_.event_callback = std::bind(&Application::EventCallback, this, std::placeholders::_1);
  layer_stack_ = std::make_unique<LayerStack>();

  is_running_ = true;
}

auto Application::Run() -> void {
  while (is_running_) {
    window_->Tick();
  }
}

auto Application::EventCallback(std::shared_ptr<Event> event) -> void {
  LOG_TRACE("{}", event->ToString());
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