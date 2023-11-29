#pragma once
#include "event/event.h"
#include "nanorpch.h"

namespace nanoR {
class Layer {
 public:
  Layer(std::string name) : name_(name) {}
  virtual auto OnAttach() -> void = 0;
  // HINT: the unit of delta_time is microseconds
  virtual auto Tick(uint64_t delta_time) -> void = 0;
  virtual auto OnDetach() -> void = 0;
  virtual auto OnEvent(Event& event) -> void = 0;
  virtual auto TickUI() -> void = 0;
  auto GetName() const -> std::string {
    return name_;
  }

 protected:
  std::string name_;
};
}  // namespace nanoR