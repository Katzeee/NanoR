#pragma once
// clang-format off
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
#include "../../common/stb_image.h"
#include "assimp/material.h"
#include "assimp/types.h"
#include "mesh.hpp"

namespace xac {

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

  void Draw() {
    for (const auto &mesh : meshes_) {
      mesh.Draw();
    }
  }

 private:
  std::vector<Mesh> meshes_;
  std::unordered_set<std::string> texture_loaded_;
  std::string path_;

  auto ProcessMesh(aiMesh *mesh, const aiScene *scene) -> Mesh {
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
    std::vector<Mesh::Texture> textures;
    if (scene->HasMaterials()) {
      auto *material = scene->mMaterials[mesh->mMaterialIndex];
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
    std::cout << textures.size() << std::endl;

    return {std::move(vertices), std::move(indices)};
  }

  std::vector<Mesh::Texture> LoadTextures(aiMaterial *material, aiTextureType type, std::string type_name) {
    std::vector<Mesh::Texture> textures;
    aiString path;
    for (auto i = 0; i < material->GetTextureCount(type); i++) {
      material->GetTexture(type, i, &path);
      if (texture_loaded_.find(path.C_Str()) == texture_loaded_.end()) {  // havn't load this texture
        texture_loaded_.insert(path.C_Str());
        textures.emplace_back(LoadTexture(&path), type_name);
      }
    }
    return textures;
  }

  auto LoadTexture(aiString *file_name) -> unsigned int {
    std::string file_path = path_ + '/' + file_name->C_Str();
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    int x, y, nchs;
    auto *image_data = stbi_load(file_path.c_str(), &x, &y, &nchs, 0);
    GLenum format;
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
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      stbi_image_free(image_data);
    } else {
      std::cout << "ERROR::LoadTex: " << file_path << std::endl;
    }

    return texture_id;
  }

  void ProcessNode(aiNode *node, const aiScene *scene) {
    for (auto i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh =
          scene->mMeshes[node->mMeshes[i]];  // scene->mMeshes stores real meshes, node->mMeshes stores indices to them
      meshes_.emplace_back(ProcessMesh(mesh, scene));
    }

    for (auto i = 0; i < node->mNumChildren; i++) {
      ProcessNode(node->mChildren[i], scene);
    }
  }
};
};  // namespace xac
