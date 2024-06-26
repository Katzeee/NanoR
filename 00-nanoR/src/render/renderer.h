#pragma once

namespace nanoR {
class RHI;
class Scene;
class Camera;
class RHIFramebuffer;
class RHIBuffer;

class Renderer {
 public:
  virtual auto Render(RHI *rhi, Scene *scene, Camera *camera, std::shared_ptr<RHIFramebuffer> framebuffer) -> void = 0;
  virtual ~Renderer() = default;

 protected:
  std::shared_ptr<RHIBuffer> ubo_engine_;
  std::shared_ptr<RHIBuffer> ubo_light_;
};

}  // namespace nanoR