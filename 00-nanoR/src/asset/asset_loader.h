#pragma once
#include "nanorpch.h"
#include "render/mesh.h"

namespace nanoR {
class AssetLoader {
public:
  static auto LoadMesh(std::string_view path) -> std::vector<Mesh>;

private:
  static auto LoadMeshInternal(const aiNode *ai_node, const aiScene *ai_scene, std::vector<Mesh> &meshes) -> void;
  static auto ParseMeshNode(const aiMesh *ai_mesh, const aiScene *ai_scene) -> Mesh;

  template <typename T>
  static auto FillMeshVertexAttr(Mesh &mesh, unsigned int count, T &attr_data, VertexAttributeType type) {
    if (!attr_data) {
      return;
    }
    if constexpr (std::is_same_v<T, aiVector3D>) {
      std::vector<glm::vec3> data;
      for (auto i = 0; i < count; ++i) {
        const auto &attr = attr_data[i];
        data.emplace_back(attr.x, attr.y, attr.z);
      }
      mesh.attributes_[type] = std::move(data);
    } else if constexpr (std::is_same_v<T, aiVector2D>) {
      std::vector<glm::vec2> data;
      for (auto i = 0; i < count; ++i) {
        const auto &attr = attr_data[i];
        data.emplace_back(attr.x, attr.y);
      }
      mesh.attributes_[type] = std::move(data);
    }
  }
};
} // namespace nanoR