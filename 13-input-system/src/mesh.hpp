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
#include "shader.hpp"

namespace xac {

class Mesh {
 public:
  struct Vertex {
    Vertex() = default;
    Vertex(glm::vec3 &&position, glm::vec3 &&normal, glm::vec2 &&texcoord)
        : position_(std::move(position)), normal_(std::move(normal)), texcoord_(std::move(texcoord)) {}
    glm::vec3 position_;
    glm::vec3 normal_;
    glm::vec2 texcoord_;
  };

  Mesh(std::vector<Vertex> &&vertices, std::vector<unsigned int> &&indices, Material &&material,
       char const *name = "null")
      : vertices_(std::move(vertices)),
        indices_(std::move(indices)),
        material_(std::make_unique<Material>(std::move(material))),
        name_(name) {
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices_.size(), indices_.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, position_)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, normal_)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, texcoord_)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);  // unbind vertex array
    // std::cout << name_ << std::endl;
  }

  void Draw() const {
    shader_->Use();
    int diffuse_count = 0;
    int specular_count = 0;
    int normal_count = 0;
    const auto &textures = material_->textures_;
    for (auto i = 0; i < textures.size(); i++) {
      const auto &texture = textures[i];
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, texture.id_);
      if (texture.type_ == "texture_diffuse") {
        shader_->SetInt("texture_diffuse" + std::to_string(diffuse_count), diffuse_count);
        diffuse_count++;
      } else if (texture.type_ == "texture_specular") {
        shader_->SetInt("texture_specular" + std::to_string(specular_count), specular_count);
        specular_count++;
      } else if (texture.type_ == "texture_normal") {
        shader_->SetInt("texture_normal" + std::to_string(normal_count), normal_count);
        normal_count++;
      }
    }
    shader_->SetVec3("Ka", material_->Ka_);
    shader_->SetVec3("Kd", material_->Kd_);
    shader_->SetVec3("Ks", material_->Ks_);
    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  void SetShader(Shader &shader) { shader_ = std::make_unique<Shader>(shader); }

 private:
  unsigned int VAO_, VBO_, EBO_;
  std::unique_ptr<Shader> shader_;
  std::vector<Vertex> vertices_;
  std::unique_ptr<Material> material_;
  std::vector<unsigned int> indices_;
  std::string name_;
  // std::vector<Texture> textures_;
};

};  // namespace xac
