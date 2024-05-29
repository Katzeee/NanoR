#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cassert>
#include <iostream>
#include <vector>

struct Mesh {
  std::string name;
};

struct GameObject {
  std::string name;
  std::vector<GameObject> children;
  std::vector<Mesh> meshes;
};

static auto LoadModelInternal(const aiNode *ai_node, const aiScene *ai_scene, GameObject &model) -> void {
  if (ai_node->mNumMeshes > 0) {
    model.meshes.emplace_back(Mesh{.name = ai_node->mName.C_Str()});
    // Process sub meshes
    for (auto i = 0; i < ai_node->mNumMeshes; i++) {
      // scene->mMeshes stores real meshes, node->mMeshes stores indices to them
      aiMesh *ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
      // meshes.emplace_back(ParseMeshNode(mesh, ai_scene));
    }
  }

  for (auto i = 0; i < ai_node->mNumChildren; i++) {
    const auto &ai_child_node = ai_node->mChildren[i];
    auto child_go = GameObject{.name = ai_child_node->mName.C_Str()};
    LoadModelInternal(ai_child_node, ai_scene, child_go);
    model.children.emplace_back(std::move(child_go));
  }
}

static auto LoadModel(std::string_view path) -> GameObject {
  // TODO: for two models in one scene
  GameObject model;
  Assimp::Importer importer;
  // unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
  unsigned int flags = aiProcess_Triangulate;
  const aiScene *ai_scene = importer.ReadFile(path.data(), flags);
  if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
    std::cout << "ERROR::ASSIMP: " << importer.GetErrorString() << std::endl;
    return model;
  }
  assert(ai_scene->mRootNode->mNumChildren == 1);
  assert(ai_scene->mRootNode->mNumMeshes == 0);
  // every node under mRootNode is a model
  for (auto i = 0; i < ai_scene->mRootNode->mNumChildren; ++i) {
    model.name = ai_scene->mRootNode->mChildren[i]->mName.C_Str();
    LoadModelInternal(ai_scene->mRootNode->mChildren[i], ai_scene, model);
  }
  // TODO: register to resouce manager
  return model;
}

int main() {
  LoadModel("../resources/models/Nahida/Avatar_Loli_Catalyst_Nahida.fbx");
}