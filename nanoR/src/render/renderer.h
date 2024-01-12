#pragma once

namespace nanoR {
class RHI;
class Scene;
class Camera;
class RHIFramebuffer;
class RHIBuffer;

class Renderer {
 public:
  virtual auto Render(RHI *rhi, Scene *scene, Camera *camera, RHIFramebuffer *framebuffer) -> void = 0;
  virtual ~Renderer() = default;

 protected:
  std::shared_ptr<RHIBuffer> ubo_matrices_;
  std::shared_ptr<RHIBuffer> ubo_vectors_;
  std::shared_ptr<RHIBuffer> ubo_per_objs_;
};

}  // namespace nanoR