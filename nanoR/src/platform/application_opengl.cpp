#include "application_opengl.h"

#include "layer/ui_layer.h"
#include "platform/opengl/rhi_opengl.h"

namespace nanoR {

auto ApplicationOpenGL::Init() -> void {
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    LOG_FATAL("Load OpenGL functions fail.\n");
    throw std::runtime_error("Load OpenGL functions fail.");
  }
  glViewport(
      0, 0, GlobalContext::Instance().window->window_prop_.width, GlobalContext::Instance().window->window_prop_.height
  );
  GlobalContext::Instance().rhi = std::make_shared<RHIOpenGL>();
  GlobalContext::Instance().ui_layer = std::make_shared<UILayer>("UILayer");
  PushOverlayLayer(GlobalContext::Instance().ui_layer);

  is_running_ = true;
}

}  // namespace nanoR