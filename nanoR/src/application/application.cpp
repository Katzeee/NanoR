#include "application.h"

#include "window/linux_window.h"

namespace nanoR {

Application::Application() {
  Init();
}

auto Application::Init() -> void {
  window_ = std::make_unique<LinuxWindow>();
  is_running_ = true;
  window_->user_data_.event_callback = std::bind(&Application::EventCallback, this, std::placeholders::_1);
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

}  // namespace nanoR