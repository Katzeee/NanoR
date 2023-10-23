#pragma once
#include "nanorpch.h"
#include "window/window.h"

namespace nanoR {

class Window;
class Event;

class Application {
 public:
  Application();
  virtual ~Application() = default;
  virtual auto Init() -> void;
  virtual auto Run() -> void;
  auto EventCallback(std::shared_ptr<Event> event) -> void;

 protected:
  std::unique_ptr<Window> window_;
  bool is_running_ = false;
};
}  // namespace nanoR