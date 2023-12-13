#pragma once
#include "event/event.h"
#include "nanorpch.h"

namespace nanoR {
class Layer {
 public:
  Layer(std::string const& name);
  virtual auto OnAttach() -> void;
  // HINT: the unit of delta_time is microseconds
  virtual auto Tick(uint64_t delta_time) -> void;
  virtual auto OnDetach() -> void;
  virtual auto OnEvent(std::shared_ptr<Event> const& event) -> void;
  virtual auto TickUI() -> void;
  auto GetName() const -> std::string;

 protected:
  std::string name_;
};
}  // namespace nanoR