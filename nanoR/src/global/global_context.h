#pragma once
#include "nanorpch.h"
#include "platform/input_system_glfw.h"
#include "window/window.h"

namespace nanoR {
class GlobalContext {
 public:
  static auto Instance() -> GlobalContext&;

  std::shared_ptr<WindowBase> window;
  std::shared_ptr<InputSystem<Platform::Linux>> input_system;
};

}  // namespace nanoR