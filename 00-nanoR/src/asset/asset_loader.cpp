#include "asset_loader.h"

namespace nanoR {

auto AssetLoader::LoadModelFromFile(std::string_view path) -> Resource {
  Resource resource;
  Assimp::Importer importer;
  // unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
  unsigned int flags = aiProcess_Triangulate;
  const aiScene *ai_scene = importer.ReadFile(path.data(), flags);
  if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
    LOG_ERROR("ERROR::ASSIMP: {}\n", importer.GetErrorString());
    return resource;
  }

  // every node under mRootNode is a model
  for (auto i = 0; i < ai_scene->mRootNode->mNumChildren; ++i) {
    resource.GetComponent<NameComponent>()->name = ai_scene->mRootNode->mChildren[i]->mName.C_Str();
    LoadModelInternal(ai_scene->mRootNode->mChildren[i], ai_scene, resource);
  }
  // TODO: register to resouce manager
  return resource;
}

auto AssetLoader::LoadModelInternal(const aiNode *ai_node, const aiScene *ai_scene, Resource &resource) -> void {
  if (ai_node->mNumMeshes > 0) {
    resource.AddComponent<MeshComponent>();
    // Process sub meshes
    for (auto i = 0; i < ai_node->mNumMeshes; i++) {
      // scene->mMeshes stores real meshes, node->mMeshes stores indices to them
      aiMesh *ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
      // meshes.emplace_back(ParseMeshNode(mesh, ai_scene));
    }
  }

  for (auto i = 0; i < ai_node->mNumChildren; i++) {
    const auto &ai_child_node = ai_node->mChildren[i];
    auto child_res = Resource(ai_child_node->mName.C_Str());
    child_res.GetComponent<TransformComponent>()->parent = resource.GetComponent<TransformComponent>();
    LoadModelInternal(ai_node->mChildren[i], ai_scene, child_res);
    resource.GetComponent<TransformComponent>()->children.emplace_back(child_res.GetComponent<TransformComponent>());
  }
}

// auto AssetLoader::ParseMeshNode(const aiMesh *ai_mesh, const aiScene *ai_scene) -> Mesh {
// Mesh mesh;
// FillMeshVertexAttr(mesh, ai_mesh->mNumVertices, ai_mesh->mVertices, VertexAttributeType::POSITION);
// FillMeshVertexAttr(mesh, ai_mesh->mNumVertices, ai_mesh->mNormals, VertexAttributeType::NORMAL);
// FillMeshVertexAttr(mesh, ai_mesh->mNumVertices, ai_mesh->mTextureCoords[0], VertexAttributeType::UV0);
// return mesh;
// }

} // namespace nanoR