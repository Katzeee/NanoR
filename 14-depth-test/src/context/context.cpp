#include "context.hpp"
#include "camera/camera.hpp"
#include "input/input_system.hpp"

xac::GlobalContext global_context;

namespace xac {

void GlobalContext::Init() {
  camera_ = std::make_unique<Camera>();
  input_system_ = std::make_unique<InputSystem>();
}

auto InCommand(xac::ControlCommand command) -> bool {
  return static_cast<uint32_t>(command) & global_context.control_commad_;
}

}  // end namespace xac
