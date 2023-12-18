#pragma once
#include "nanorpch.h"
#include "render/rhi.h"
#include "render/rhi_type.h"
#include "resource_type.h"

namespace nanoR {

class ResourceManager {
 public:
  static auto LoadMeshData() -> MeshData;
  static auto LoadShaderData(char const *vs_path, char const *fs_path) -> ShaderData;
  // TODO: not create like this
  static auto GetUnlitShader(RHI *rhi) -> std::shared_ptr<RHIShaderProgram>;
  static auto GetLitShader(RHI *rhi) -> std::shared_ptr<RHIShaderProgram>;
  static auto LoadTextureFromFile(std::string_view file_path) -> unsigned int;
  static auto GetQuadMeshData() -> MeshData;

 private:
  static auto ReadFromFile(char const *file_path) -> std::string;
};

class Model {
 public:
  Model() = delete;
  Model(const char *path) : path_(path) {
    path_ = path_.substr(0, path_.find_last_of('/'));
    Assimp::Importer importer;
    const aiScene *scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      std::cout << "ERROR::ASSIMP: " << importer.GetErrorString() << std::endl;
      return;
    }
    ProcessNode(scene->mRootNode, scene);
  }

 public:
  std::vector<MeshData> meshes_;
  std::unordered_set<std::string> texture_loaded_;
  std::string path_;

  auto LoadMesh(aiMesh *mesh, const aiScene *scene) -> MeshData {
    // load vertices
    std::vector<MeshData::Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      MeshData::Vertex vertex;
      vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
      if (mesh->mNormals) {
        vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
      }
      if (mesh->mTextureCoords[0]) {
        vertex.texcoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
      } else {
        vertex.texcoord = {0.0f, 0.0f};
      }
      vertices.emplace_back(vertex);
    }

    // load indices
    std::vector<unsigned int> indices;
    indices.reserve(mesh->mNumFaces * 3);

    for (auto i = 0; i < mesh->mNumFaces; i++) {
      auto face = mesh->mFaces[i];
      for (auto j = 0; j < face.mNumIndices; j++) {
        indices.emplace_back(face.mIndices[j]);
      }
    }

    // load materials
    // std::vector<Texture> textures;
    // aiColor3D ka;
    // aiColor3D ks;
    // aiColor3D kd;
    // if (scene->HasMaterials()) {
    //   auto *material = scene->mMaterials[mesh->mMaterialIndex];
    //   // load Ka, Ks, Kd...
    //   material->Get(AI_MATKEY_COLOR_AMBIENT, ka);
    //   material->Get(AI_MATKEY_COLOR_DIFFUSE, kd);
    //   material->Get(AI_MATKEY_COLOR_SPECULAR, ks);

    //   auto diffuse_mats = LoadTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    //   textures.insert(
    //       textures.end(), std::make_move_iterator(diffuse_mats.begin()), std::make_move_iterator(diffuse_mats.end())
    //   );
    //   auto specular_mats = LoadTextures(material, aiTextureType_SPECULAR, "texture_specular");
    //   textures.insert(
    //       textures.end(), std::make_move_iterator(specular_mats.begin()),
    //       std::make_move_iterator(specular_mats.end())
    //   );
    //   auto normal_mats = LoadTextures(material, aiTextureType_NORMALS, "texture_normal");
    //   textures.insert(
    //       textures.end(), std::make_move_iterator(normal_mats.begin()), std::make_move_iterator(normal_mats.end())
    //   );
    // }

    return {std::move(vertices), std::move(indices)};
  }

  // auto LoadTextures(aiMaterial *material, aiTextureType type, const std::string &type_name) -> std::vector<Texture> {
  //   std::vector<Texture> textures;
  //   aiString path;
  //   for (auto i = 0; i < material->GetTextureCount(type); i++) {
  //     material->GetTexture(type, i, &path);
  //     if (texture_loaded_.find(path.C_Str()) == texture_loaded_.end()) {  // if havn't load this texture
  //       texture_loaded_.insert(path.C_Str());
  //       auto &&texture_id = LoadTexture(&path);
  //       Texture texture{texture_id, type_name};
  //       textures.emplace_back(std::move(texture));
  //       // aiTextureMapMode map_mode;
  //       // material->Get(AI_MATKEY_MAPPINGMODE_U(type, i), map_mode);
  //       // std::cout << map_mode << " ";
  //       // material->Get(AI_MATKEY_MAPPINGMODE_V(type, i), map_mode);
  //       // std::cout << map_mode << std::endl;
  //     }
  //   }
  //   return textures;
  // }

  // auto LoadTexture(aiString *file_name) -> unsigned int {
  //   std::string file_path = path_ + '/' + file_name->C_Str();
  //   return LoadTextureFromFile(file_path);
  // }

  void ProcessNode(aiNode *node, const aiScene *scene) {
    for (auto i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh =
          scene->mMeshes[node->mMeshes[i]];  // scene->mMeshes stores real meshes, node->mMeshes stores indices to them
      meshes_.emplace_back(LoadMesh(mesh, scene));
    }

    for (auto i = 0; i < node->mNumChildren; i++) {
      ProcessNode(node->mChildren[i], scene);
    }
  }
};

}  // namespace nanoR