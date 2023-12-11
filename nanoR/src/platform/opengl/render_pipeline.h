#pragma once
#include "nanorpch.h"
#include "render_resource_opengl.h"
#include "rhi_opengl.h"

namespace nanoR {
class RenderPipeline {
 public:
  auto Render(RHI* rhi, OpenGLMesh* mesh) -> void;
};
}  // namespace nanoR