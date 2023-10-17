#pragma once
// clang-format off
#include <asm-generic/errno.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <GL/gl.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <iostream>
#include <iterator>
#include <unordered_set>
#include <utility>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "assimp/material.h"
#include "assimp/types.h"
#include "material.hpp"
#include "mesh.hpp"
#include "stb_image.h"

namespace xac {

static auto LoadCubemapFromFile(std::vector<std::string> file_paths) -> unsigned int {
  unsigned int texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

  int x;
  int y;
  int nchs;
  for (auto i = 0; i < file_paths.size(); i++) {
    unsigned char *image_data = stbi_load(file_paths[i].c_str(), &x, &y, &nchs, 0);
    if (image_data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
      std::cout << "LoadTex: " << file_paths[i] << ", channels: " << nchs << std::endl;
    } else {
      std::cout << "ERROR::LoadTex: " << file_paths[i] << std::endl;
    }
    stbi_image_free(image_data);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  return texture_id;
}

static auto LoadTextureFromFile(std::string_view file_path) -> unsigned int {
  unsigned int texture_id;
  glGenTextures(1, &texture_id);
  int x;
  int y;
  int nchs;
  auto *image_data = stbi_load(file_path.data(), &x, &y, &nchs, 0);
  GLenum format = 0;
  if (image_data) {
    switch (nchs) {
      case 1:
        format = GL_RED;
        break;
      case 3:
        format = GL_RGB;
        break;
      case 4:
        format = GL_RGBA;
        break;
      default:
        break;
    }
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, x, y, 0, format, GL_UNSIGNED_BYTE, image_data);
    glGenerateTextureMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    stbi_image_free(image_data);
    std::cout << "LoadTex: " << file_path << ", channels: " << nchs << std::endl;
  } else {
    std::cout << "ERROR::LoadTex: " << file_path << std::endl;
  }
  return texture_id;
}

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

  void SetShader(std::shared_ptr<Shader> const &shader) {
    for (auto &mesh : meshes_) {
      mesh.SetShader(shader);
    }
  }

  void Draw() const {
    for (const auto &mesh : meshes_) {
      mesh.Draw();
    }
  }

 private:
  std::vector<Mesh> meshes_;
  std::unordered_set<std::string> texture_loaded_;
  std::string path_;

  auto LoadMesh(aiMesh *mesh, const aiScene *scene) -> Mesh {
    // load vertices
    std::vector<Mesh::Vertex> vertices;
    vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Mesh::Vertex vertex;
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
    std::vector<Texture> textures;
    aiColor3D ka;
    aiColor3D ks;
    aiColor3D kd;
    if (scene->HasMaterials()) {
      auto *material = scene->mMaterials[mesh->mMaterialIndex];
      // load Ka, Ks, Kd...
      material->Get(AI_MATKEY_COLOR_AMBIENT, ka);
      material->Get(AI_MATKEY_COLOR_DIFFUSE, kd);
      material->Get(AI_MATKEY_COLOR_SPECULAR, ks);

      auto diffuse_mats = LoadTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
      textures.insert(textures.end(), std::make_move_iterator(diffuse_mats.begin()),
                      std::make_move_iterator(diffuse_mats.end()));
      auto specular_mats = LoadTextures(material, aiTextureType_SPECULAR, "texture_specular");
      textures.insert(textures.end(), std::make_move_iterator(specular_mats.begin()),
                      std::make_move_iterator(specular_mats.end()));
      auto normal_mats = LoadTextures(material, aiTextureType_NORMALS, "texture_normal");
      textures.insert(textures.end(), std::make_move_iterator(normal_mats.begin()),
                      std::make_move_iterator(normal_mats.end()));
    }

    return {std::move(vertices), std::move(indices),
            Material(glm::vec3(ka.r, ka.g, ka.b), glm::vec3(kd.r, kd.g, kd.b), glm::vec3(ks.r, ks.g, ks.b),
                     std::move(textures)),
            mesh->mName.C_Str()};
  }

  auto LoadTextures(aiMaterial *material, aiTextureType type, const std::string &type_name) -> std::vector<Texture> {
    std::vector<Texture> textures;
    aiString path;
    for (auto i = 0; i < material->GetTextureCount(type); i++) {
      material->GetTexture(type, i, &path);
      if (texture_loaded_.find(path.C_Str()) == texture_loaded_.end()) {  // if havn't load this texture
        texture_loaded_.insert(path.C_Str());
        auto &&texture_id = LoadTexture(&path);
        Texture texture{texture_id, type_name};
        textures.emplace_back(std::move(texture));
        // aiTextureMapMode map_mode;
        // material->Get(AI_MATKEY_MAPPINGMODE_U(type, i), map_mode);
        // std::cout << map_mode << " ";
        // material->Get(AI_MATKEY_MAPPINGMODE_V(type, i), map_mode);
        // std::cout << map_mode << std::endl;
      }
    }
    return textures;
  }

  auto LoadTexture(aiString *file_name) -> unsigned int {
    std::string file_path = path_ + '/' + file_name->C_Str();
    return LoadTextureFromFile(file_path);
  }

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
};  // namespace xac
