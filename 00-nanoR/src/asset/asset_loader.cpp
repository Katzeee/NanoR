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
    auto &mesh = resource.AddComponent<MeshComponent>()->mesh;
    mesh = std::make_shared<Mesh>();
    // Process sub meshes
    for (auto i = 0; i < ai_node->mNumMeshes; i++) {
      // scene->mMeshes stores real meshes, node->mMeshes stores indices to them
      aiMesh *ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
      mesh->submeshes_.emplace_back(ParseMeshNode(ai_mesh, ai_scene));
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

auto AssetLoader::ParseMeshNode(const aiMesh *ai_mesh, const aiScene *ai_scene) -> SubMesh {
  SubMesh submesh;
  for (auto i = 0; i < ai_mesh->mNumVertices; i++) {
    Vertex vertex;
    auto &position = ai_mesh->mVertices[i];
    vertex.position = glm::vec3(position.x, position.y, position.z);
    auto &normal = ai_mesh->mNormals[i];
    vertex.normal = glm::vec3(normal.x, normal.y, normal.z);
    if (ai_mesh->mTangents) {
      auto &tangent = ai_mesh->mTangents[i];
      vertex.tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
    }
    // TODO: UV3 and more
    for (int j = 0; j < 2; ++j) {
      if (ai_mesh->mTextureCoords[j]) {
        auto &uv = ai_mesh->mTextureCoords[j][i];
        // TODO: UV maya be a 1D or 3D vector, check ai_mesh->mNumUVComponents
        vertex.uv[j] = glm::vec2(uv.x, uv.y);
      }
    }
    submesh.vertices_.emplace_back(std::move(vertex));
  }
  return submesh;
}

} // namespace nanoR