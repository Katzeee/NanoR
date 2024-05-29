#include "asset_loader.h"

namespace nanoR {

auto AssetLoader::LoadMesh(std::string_view path) -> std::vector<Mesh> {
  std::vector<Mesh> meshes;
  Assimp::Importer importer;
  // unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
  unsigned int flags = aiProcess_Triangulate;
  const aiScene *scene = importer.ReadFile(path.data(), flags);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    LOG_ERROR("ERROR::ASSIMP: {}", importer.GetErrorString());
    return meshes;
  }
  LoadMeshInternal(scene->mRootNode, scene, meshes);
  // TODO: register to resouce manager
  return meshes;
}

auto AssetLoader::LoadMeshInternal(const aiNode *ai_node, const aiScene *ai_scene, std::vector<Mesh> &meshes) -> void {
  for (auto i = 0; i < ai_node->mNumMeshes; i++) {
    // scene->mMeshes stores real meshes, node->mMeshes stores indices to them
    aiMesh *mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
    meshes.emplace_back(ParseMeshNode(mesh, ai_scene));
  }

  for (auto i = 0; i < ai_node->mNumChildren; i++) {
    LoadMeshInternal(ai_node->mChildren[i], ai_scene, meshes);
  }
}

auto AssetLoader::ParseMeshNode(const aiMesh *ai_mesh, const aiScene *ai_scene) -> Mesh {
  Mesh mesh;
  FillMeshVertexAttr(mesh, ai_mesh->mNumVertices, ai_mesh->mVertices, VertexAttributeType::POSITION);
  FillMeshVertexAttr(mesh, ai_mesh->mNumVertices, ai_mesh->mNormals, VertexAttributeType::NORMAL);
  FillMeshVertexAttr(mesh, ai_mesh->mNumVertices, ai_mesh->mTextureCoords[0], VertexAttributeType::UV0);
  return mesh;
}

} // namespace nanoR