#include "application_opengl.h"

#include "input_system_glfw.h"

namespace nanoR {

auto ApplicationOpenGL::Init() -> void {
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    LOG_FATAL("Load OpenGL functions fail.\n");
    throw std::runtime_error("Load OpenGL functions fail.");
  }
  glViewport(
      0, 0, GlobalContext::Instance().window->window_prop_.width, GlobalContext::Instance().window->window_prop_.height
  );
  auto input_system = std::make_unique<InputSystem<Platform::Linux>>();
  layer_stack_ = std::make_unique<LayerStack>();
  ui_layer_ = std::make_shared<UILayerGLFWOpenGL3>();
  PushOverlayLayer(ui_layer_);
  GlobalContext::Instance().ui_layer = ui_layer_;

  is_running_ = true;
}

}  // namespace nanoR