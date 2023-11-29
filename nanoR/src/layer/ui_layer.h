#pragma once
#include "layer.h"

namespace nanoR {
class UILayer : public Layer {
 public:
  UILayer(std::string name) : Layer(name) {}
  auto Tick(uint64_t delta_time) -> void override final {}
  auto TickUI() -> void override final {}

  virtual auto Begin() -> void = 0;
  virtual auto End() -> void = 0;
};
}  // namespace nanoR