#pragma once
#include "render/rhi.h"
#include "render/rhi_type.h"
#include "resource_type.h"

namespace nanoR {

class ResourceManager {
 public:
  static auto LoadMeshData() -> MeshData;
  static auto LoadShaderData(char const *vs_path, char const *fs_path) -> ShaderData;
  // TODO: read from file
  static auto GetCubeMeshData() -> MeshData;
  // TODO: not create like this
  static auto GetUnlitShader(RHI *rhi) -> std::shared_ptr<RHIShaderProgram>;

 private:
  static auto ReadFromFile(char const *file_path) -> std::string;
};

}  // namespace nanoR