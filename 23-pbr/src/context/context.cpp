#include "context.hpp"

#include "camera/camera.hpp"
#include "imgui_layer/imgui_layer.hpp"
#include "input/input_system.hpp"
#include "window/window.hpp"

xac::GlobalContext global_context;

namespace xac {

void GlobalContext::Init() {
  camera_ = std::make_unique<Camera>(glm::vec3{0, 10, 25}, glm::vec3{0, 0, 0});
  window_ = std::make_unique<Window>();
  input_system_ = std::make_unique<InputSystem>();
  input_system_->Init(window_->GetRawWindow());
  imgui_layer_ = std::make_unique<ImguiLayer>();
  imgui_layer_->Init(window_->GetRawWindow());
}

auto InCommand(xac::ControlCommand command) -> bool {
  return static_cast<uint32_t>(command) & global_context.control_commad_;
}

}  // end namespace xac
