#pragma once
#include "layer/layer_stack.h"
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
  auto PushLayer(std::shared_ptr<Layer> layer) -> void;
  auto PushOverlayLayer(std::shared_ptr<Layer> layer) -> void;
  auto PopLayer(std::shared_ptr<Layer> layer) -> void;
  auto GetLayerStack() -> LayerStack&;

 protected:
  std::unique_ptr<Window> window_;
  std::unique_ptr<LayerStack> layer_stack_;
  bool is_running_ = false;
};
}  // namespace nanoR