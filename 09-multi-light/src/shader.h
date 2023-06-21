#pragma once
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

namespace xac {

class Shader {
public:
  Shader() = delete;
  Shader(const char *vs_path, const char *fs_path);
  ~Shader();
  void Use() const;
  void SetBool(const std::string &name, bool value) const;
  void SetInt(const std::string &name, int value) const;
  void SetFloat(const std::string &name, float value) const;
  void SetMat4(const std::string &name, const glm::mat4 &mat) const;
  void SetVec3(const std::string &name, const glm::vec3 &vec) const;
  [[nodiscard]] auto GetId() const -> const unsigned int & { return id_; }

private:
  static auto ReadFromFile(const char *file_path) -> std::string;
  static void CompileShader(unsigned int shader_id);

private:
  unsigned int id_;
};

auto Shader::ReadFromFile(const char *file_path) -> std::string {
  std::ifstream fs;
  std::stringstream ss;
  fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    fs.open(file_path);
    ss << fs.rdbuf();
    fs.close();
  } catch (std::exception &e) {
    // throw e;
    std::cout << "ERROR::SHADER::READ_FAILED\n" << file_path << std::endl;
    exit(3);
  }
  return ss.str();
}

void Shader::CompileShader(unsigned int shader_id) {
  glCompileShader(shader_id);
  int success;
  char info[512];
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!static_cast<bool>(success)) {
    glGetShaderInfoLog(shader_id, 512, nullptr, info);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << info << std::endl;
  };
}

Shader::Shader(const char *vs_path, const char *fs_path) {
  std::string vs_src_str(ReadFromFile(vs_path));
  std::string fs_src_str(ReadFromFile(fs_path));
  const char *vs_src = vs_src_str.c_str();
  const char *fs_src = fs_src_str.c_str();

  unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
  unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(vs, 1, &vs_src, nullptr);
  glShaderSource(fs, 1, &fs_src, nullptr);

  CompileShader(vs);
  CompileShader(fs);

  id_ = glCreateProgram();
  glAttachShader(id_, vs);
  glAttachShader(id_, fs);
  glLinkProgram(id_);

  int success;
  char info[512];
  glGetProgramiv(id_, GL_LINK_STATUS, &success);
  if (!static_cast<bool>(success)) {
    glGetProgramInfoLog(id_, 512, nullptr, info);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << info << std::endl;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);
}

void Shader::Use() const { glUseProgram(id_); }

void Shader::SetBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), static_cast<int>(value));
}

void Shader::SetInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}
void Shader::SetFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &mat4) const {

  glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(mat4));
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &vec3) const {

  glUniform3f(glGetUniformLocation(id_, name.c_str()), vec3.x, vec3.y, vec3.z);
}

Shader::~Shader() { glDeleteProgram(id_); }
} // end namespace xac
