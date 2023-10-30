#pragma once
#include "nanorpch.h"
#include "window/window.h"

namespace nanoR {
class GlobalContext {
 public:
  static auto Instance() -> GlobalContext&;

  std::shared_ptr<WindowBase> window_;
};

}  // namespace nanoR