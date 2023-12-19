#pragma once
#include "layer.h"

namespace nanoR {
class UILayer : public Layer {
 public:
  UILayer(std::string name) : Layer(name) {}
  auto Tick(uint64_t delta_time) -> void override final;
  auto TickUI() -> void override final {}
  auto SetBlockEvent(bool block) -> void {
    passthough_event_ = !block;
  }

  auto OnAttach() -> void override;
  auto OnDetach() -> void override;
  auto OnEvent(std::shared_ptr<Event> const& event) -> bool override;

 protected:
  bool passthough_event_ = true;
  auto Begin() -> void;
  auto End() -> void;
};
}  // namespace nanoR