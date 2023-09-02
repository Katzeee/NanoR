#include "context.hpp"
#include "camera/camera.hpp"
#include "input/input_system.hpp"

int global_width = 1300;
int global_height = 600;
constexpr int global_imgui_width = 340;

namespace xac {
uint32_t GlobalContext::control_commad_ = 0;

void GlobalContext::Init() {
  camera_ = std::make_unique<Camera>();
  input_system_ = std::make_unique<InputSystem>();
}
}  // end namespace xac

xac::GlobalContext global_context;