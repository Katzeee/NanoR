#pragma once
#include "render/renderer.h"

namespace nanoR {
class RHI;
class Scene;
class Camera;
class RHIFramebuffer;
class RHIBuffer;

class RendererOpenGL : public Renderer {
 public:
  explicit RendererOpenGL(RHI *rhi);
  auto Render(RHI *rhi, Scene *scene, Camera *camera, RHIFramebuffer *framebuffer) -> void override;

 private:
  auto PrepareUniforms(RHI *rhi, Camera *camera) -> void;
};

}  // namespace nanoR