#pragma once
#include "layer/layer_stack.h"
#include "nanorpch.h"

namespace nanoR {

class WindowBase;
class Event;
class Scene;

class Application {
 public:
  Application();
  virtual ~Application() = default;
  virtual auto Init() -> void;
  virtual auto Run() -> void;
  virtual auto EventCallback(std::shared_ptr<Event> const& event) -> void;
  auto PushLayer(std::shared_ptr<Layer> const& layer) -> void;
  auto PushOverlayLayer(std::shared_ptr<Layer> const& layer) -> void;
  auto PopLayer(std::shared_ptr<Layer> const& layer) -> void;
  auto GetLayerStack() -> LayerStack&;

 protected:
  std::unique_ptr<LayerStack> layer_stack_;
  std::shared_ptr<Scene> scene_;
  bool is_running_ = false;
};
}  // namespace nanoR