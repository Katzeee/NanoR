#pragma once
#include "resource_type.h"

namespace nanoR {

class ResourceManager {
 public:
  static auto LoadMeshData() -> MeshData;
  static auto LoadShaderData(char const *vs_path, char const *fs_path) -> ShaderData;

 private:
  static auto ReadFromFile(char const *file_path) -> std::string;
};

}  // namespace nanoR