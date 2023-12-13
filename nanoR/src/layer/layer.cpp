#include "layer.h"

namespace nanoR {

Layer::Layer(std::string const& name) : name_(name) {}

auto Layer::OnAttach() -> void {}

auto Layer::Tick(uint64_t delta_time) -> void {}

auto Layer::OnDetach() -> void {}

auto Layer::OnEvent(std::shared_ptr<Event> const& event) -> void {}

auto Layer::TickUI() -> void {}

auto Layer::GetName() const -> std::string {
  return name_;
}

}  // namespace nanoR