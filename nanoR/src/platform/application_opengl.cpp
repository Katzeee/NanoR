#include "application_opengl.h"

namespace nanoR {

auto ApplicationOpenGL::Init() -> void {
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    LOG_FATAL("Load OpenGL functions fail.\n");
    throw std::runtime_error("Load OpenGL functions fail.");
  }
  glViewport(
      0, 0, GlobalContext::Instance().window_->window_prop_.width,
      GlobalContext::Instance().window_->window_prop_.height
  );
  layer_stack_ = std::make_unique<LayerStack>();
  ui_layer_ = std::make_shared<UILayerGLFWOpenGL3>();
  PushOverlayLayer(ui_layer_);

  is_running_ = true;
}

}  // namespace nanoR