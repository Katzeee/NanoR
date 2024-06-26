#include "asset_loader.h"
#include "resource/mesh.h"
#include "resource/model.h"

namespace nanoR {

auto AssetLoader::LoadModelFromFile(std::string_view path) -> Model {
  Model model(path);
  Assimp::Importer importer;
  // unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
  unsigned int flags = aiProcess_Triangulate;
  const aiScene *ai_scene = importer.ReadFile(path.data(), flags);
  if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
    LOG_ERROR("ERROR::ASSIMP: {}\n", importer.GetErrorString());
    return model;
  }

  // every node under mRootNode is a model
  for (auto i = 0; i < ai_scene->mRootNode->mNumChildren; ++i) {
    model.root_.name_ = ai_scene->mRootNode->mChildren[i]->mName.C_Str();
    // model.name_ = ai_scene->mRootNode->mChildren[i]->mName.C_Str();
    LoadModelInternal(ai_scene->mRootNode->mChildren[i], ai_scene, model.root_);
  }
  // TODO: register to resouce manager
  return model;
}

auto AssetLoader::LoadModelInternal(const aiNode *ai_node, const aiScene *ai_scene, ModelPart &parent) -> void {
  if (ai_node->mNumMeshes > 0) {
    auto &mesh = parent.mesh_.mesh;
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
    auto child = ModelPart(ai_child_node->mName.C_Str());
    child.transform_.parent = &parent.transform_;
    LoadModelInternal(ai_node->mChildren[i], ai_scene, child);
    parent.transform_.children.emplace_back(&child.transform_);
  }
}

auto AssetLoader::ParseMeshNode(const aiMesh *ai_mesh, const aiScene *ai_scene) -> SubMesh {
  SubMesh submesh;
  submesh.vertices_.reserve(ai_mesh->mNumVertices);
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
#define REPEAT_COMMAND(X)                     \
  if (ai_mesh->mTextureCoords[X]) {           \
    auto &uv = ai_mesh->mTextureCoords[X][i]; \
    vertex.uv##X = glm::vec2(uv.x, uv.y);     \
  }
    // TODO: UV may be a 1D or 3D vector, check ai_mesh->mNumUVComponents
    REPEAT_COMMAND(0)
    REPEAT_COMMAND(1)
#undef REPEAT_COMMAND
    submesh.vertices_.emplace_back(std::move(vertex));
  }
  // load indices
  submesh.indices_.reserve(ai_mesh->mNumFaces * 3);
  for (auto i = 0; i < ai_mesh->mNumFaces; ++i) {
    auto face = ai_mesh->mFaces[i];
    for (auto j = 0; j < face.mNumIndices; j++) {
      submesh.indices_.emplace_back(face.mIndices[j]);
    }
  }
  return submesh;
}

} // namespace nanoR