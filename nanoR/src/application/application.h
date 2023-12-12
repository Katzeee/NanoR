#pragma once
#include "layer/layer_stack.h"
#include "layer/ui_layer.h"
#include "nanorpch.h"
#include "window/window.h"

namespace nanoR {

class WindowBase;
class Event;

class Application {
 public:
  Application();
  virtual ~Application() = default;
  virtual auto Init() -> void;
  virtual auto Run() -> void;
  virtual auto EventCallback(std::shared_ptr<Event> const& event) -> void;
  auto PushLayer(std::shared_ptr<Layer> layer) -> void;
  auto PushOverlayLayer(std::shared_ptr<Layer> layer) -> void;
  auto PopLayer(std::shared_ptr<Layer> layer) -> void;
  auto GetLayerStack() -> LayerStack&;

 protected:
  std::unique_ptr<LayerStack> layer_stack_;
  std::shared_ptr<UILayer> ui_layer_;
  bool is_running_ = false;
};
}  // namespace nanoR