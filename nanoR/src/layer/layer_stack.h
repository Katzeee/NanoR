#pragma once
#include "layer.h"
#include "nanorpch.h"

namespace nanoR {
class LayerStack {
 public:
  auto PushLayer(std::shared_ptr<Layer> layer) -> void;
  auto PushOverlayLayer(std::shared_ptr<Layer> layer) -> void;
  auto PopLayer(std::shared_ptr<Layer> layer) -> void;
  auto GetLayers() -> std::deque<std::shared_ptr<Layer>>&;

  auto ToString() -> std::string;

 private:
  // std::shared_ptr<Layer> first_overlay_layer_;
  int last_overlay_layer_ = -1;  // used for push overlay and pop
  std::deque<std::shared_ptr<Layer>> layers_;
};
}  // namespace nanoR