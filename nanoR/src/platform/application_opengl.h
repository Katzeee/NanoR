#pragma once
#include "application/application.h"
#include "global/global_context.h"
#include "layer/layer.h"
#include "layer/layer_stack.h"
#include "window/window.h"

namespace nanoR {

class ApplicationOpenGL : public Application {
 public:
  ApplicationOpenGL() {
    Init();
  }

  auto Init() -> void override;
};
}  // namespace nanoR