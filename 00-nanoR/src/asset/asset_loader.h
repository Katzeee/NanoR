#pragma once
#include "nanorpch.h"
#include "resource/mesh.h"

namespace nanoR {

class Model;
class ModelPart;

class AssetLoader {
public:
  static auto LoadModelFromFile(std::string_view path) -> Model;

private:
  static auto LoadModelInternal(const aiNode *ai_node, const aiScene *ai_scene, ModelPart &model_part) -> void;
  static auto ParseMeshNode(const aiMesh *ai_mesh, const aiScene *ai_scene) -> SubMesh;

  // template <typename T>
  // static auto FillMeshVertexAttr(Mesh &mesh, unsigned int count, T &attr_data, VertexAttributeType type) {
  //   if (!attr_data) {
  //     return;
  //   }
  //   if constexpr (std::is_same_v<T, aiVector3D>) {
  //     std::vector<glm::vec3> data;
  //     for (auto i = 0; i < count; ++i) {
  //       const auto &attr = attr_data[i];
  //       data.emplace_back(attr.x, attr.y, attr.z);
  //     }
  //     mesh.attributes_[type] = std::move(data);
  //   } else if constexpr (std::is_same_v<T, aiVector2D>) {
  //     std::vector<glm::vec2> data;
  //     for (auto i = 0; i < count; ++i) {
  //       const auto &attr = attr_data[i];
  //       data.emplace_back(attr.x, attr.y);
  //     }
  //     mesh.attributes_[type] = std::move(data);
  //   }
  // }
};
} // namespace nanoR