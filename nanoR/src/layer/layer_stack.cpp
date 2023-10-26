#include "layer_stack.h"

namespace nanoR {

auto LayerStack::PushLayer(std::shared_ptr<Layer> layer) -> void {
  layers_.insert(layers_.begin() + last_overlay_layer_ + 1, layer);
  layer->OnAttach();
}

auto LayerStack::PushOverlayLayer(std::shared_ptr<Layer> layer) -> void {
  layers_.emplace_front(layer);
  last_overlay_layer_++;
  layer->OnAttach();
}

auto LayerStack::PopLayer(std::shared_ptr<Layer> layer) -> void {
  if (auto layer_it = std::find(layers_.begin(), layers_.end(), layer); layer_it != layers_.end()) {
    auto diff = layer_it - layers_.begin();
    if (diff <= last_overlay_layer_) {
      last_overlay_layer_--;
    }
    layers_.erase(layer_it);
    layer->OnDetach();
  }
}

auto LayerStack::GetLayers() -> std::deque<std::shared_ptr<Layer>>& {
  return layers_;
}

auto LayerStack::ToString() -> std::string {
  return fmt::format("LayerStack: [{}]",
                     fmt::join(std::views::transform(layers_, [](auto&& layer) { return layer->GetName(); }), ", "));
  // return fmt::format("{}", fmt::join(, ", "));
}

}  // namespace nanoR