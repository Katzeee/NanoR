#pragma once
#include "mesh_data.h"

namespace nanoR {

class ResourceManager {
 public:
  static auto LoadMeshData() -> MeshData;
};

}  // namespace nanoR