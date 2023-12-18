#pragma once
#include "layer.h"

namespace nanoR {
class UILayer : public Layer {
 public:
  UILayer(std::string name) : Layer(name) {}
  auto Tick(uint64_t delta_time) -> void override final {}
  auto TickUI() -> void override final {}
  auto SetBlockEvent(bool block) -> void {
    LOG_TRACE("block: {}\n", block);
    passthough_event_ = !block;
  }

  virtual auto Begin() -> void = 0;
  virtual auto End() -> void = 0;

 protected:
  bool passthough_event_ = true;
};
}  // namespace nanoR