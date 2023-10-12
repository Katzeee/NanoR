#pragma once
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "material.hpp"
#include "shader/shader.hpp"

namespace xac {

class Mesh {
 public:
  struct Vertex {
    Vertex() = default;
    Vertex(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &texcoord)
        : position(position), normal(normal), texcoord(texcoord) {}

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
  };

  Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const Material &material,
       char const *name = "null")
      : vertices_(vertices), indices_(indices), material_(std::make_unique<Material>(material)), name_(name) {
    Init();
  }

  Mesh(std::vector<Vertex> &&vertices, std::vector<unsigned int> &&indices, Material &&material,
       char const *name = "null")
      : vertices_(std::move(vertices)),
        material_(std::make_unique<Material>(std::move(material))),
        indices_(std::move(indices)),
        name_(name) {
    Init();
  }

  Mesh(const Mesh &mesh) : Mesh(mesh.vertices_, mesh.indices_, *mesh.material_, mesh.name_.c_str()) {}

  void Init() {
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    glGenVertexArrays(1, &vao_);
    // HINT: must bind vao first
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices_.size(), indices_.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, texcoord)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);  // unbind vertex array
  }

  void Draw() const {
    shader_->Use();
    int diffuse_count = 0;
    int specular_count = 0;
    int normal_count = 0;
    const auto &textures = material_->textures;
    for (auto i = 0; i < textures.size(); i++) {
      const auto &texture = textures[i];
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, texture.id);
      if (texture.type == "texture_diffuse") {
        shader_->SetInt("texture_diffuse" + std::to_string(diffuse_count), diffuse_count);
        diffuse_count++;
      } else if (texture.type == "texture_specular") {
        shader_->SetInt("texture_specular" + std::to_string(specular_count), specular_count);
        specular_count++;
      } else if (texture.type == "texture_normal") {
        shader_->SetInt("texture_normal" + std::to_string(normal_count), normal_count);
        normal_count++;
      }
    }
    shader_->SetVec3("Ka", material_->ka);
    shader_->SetVec3("Kd", material_->kd);
    shader_->SetVec3("Ks", material_->ks);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }

  void SetShader(std::shared_ptr<Shader> const &shader) { shader_ = shader; }

 private:
  unsigned int vao_, vbo_, ebo_;
  std::shared_ptr<Shader> shader_;
  std::vector<Vertex> vertices_;
  std::unique_ptr<Material> material_;
  std::vector<unsigned int> indices_;
  std::string name_;
  // std::vector<Texture> textures_;
};
};  // namespace xac
