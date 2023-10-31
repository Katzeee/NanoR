#include "application_opengl.h"

namespace nanoR {

auto ApplicationOpenGL::Init() -> void {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_FATAL("Load OpenGL functions fail.\n");
    throw std::runtime_error("Load OpenGL functions fail.");
  }
  layer_stack_ = std::make_unique<LayerStack>();
  ui_layer_ = std::make_shared<UILayerGLFWOpenGL3>();
  PushOverlayLayer(ui_layer_);

  is_running_ = true;
}

}  // namespace nanoR