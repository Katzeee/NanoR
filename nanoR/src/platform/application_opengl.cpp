#include "application_opengl.h"
namespace nanoR {

auto ApplicationOpenGL::Init() -> void {
  layer_stack_ = std::make_unique<LayerStack>();
  ui_layer_ = std::make_shared<UILayerGLFWOpenGL3>();
  PushOverlayLayer(ui_layer_);

  is_running_ = true;
}

}  // namespace nanoR