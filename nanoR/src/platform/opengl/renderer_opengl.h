#pragma once

namespace nanoR {
class RHI;
class Scene;
class Camera;
class RHIFramebuffer;

class Renderer {
 public:
  static auto Render(RHI *rhi, Scene *scene, Camera *camera, RHIFramebuffer *framebuffer) -> void;
};

}  // namespace nanoR