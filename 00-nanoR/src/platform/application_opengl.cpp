#include "application_opengl.h"

#include "layer/ui_layer.h"
#include "platform/opengl/renderer_opengl.h"
#include "render/rhi_opengl4.h"

namespace nanoR {

auto ApplicationOpenGL::Init() -> void {
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    LOG_FATAL("Load OpenGL functions fail.\n");
    throw std::runtime_error("Load OpenGL functions fail.");
  }
  glViewport(
      0, 0, GlobalContext::Instance().window->window_prop_.width, GlobalContext::Instance().window->window_prop_.height
  );
  glEnable(GL_DEPTH_TEST);
  auto *rhi = new RHIOpenGL4();
  GlobalContext::Instance().rhi.reset(rhi);
  GlobalContext::Instance().ui_layer = std::make_shared<UILayer>("UILayer");
  PushOverlayLayer(GlobalContext::Instance().ui_layer);
  GlobalContext::Instance().resource_manager = std::make_shared<ResourceManager>();
  GlobalContext::Instance().renderer = std::make_shared<RendererOpenGL>(rhi);

  is_running_ = true;
}

}  // namespace nanoR