#pragma once
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>

namespace xac {

class Shader {
public:
  Shader() = delete;
  Shader(const char *vs_path, const char *ps_path);
  ~Shader();
  void Use() const;
  void SetBool(const std::string &name, bool value) const;
  void SetInt(const std::string &name, int value) const;
  void SetFloat(const std::string &name, float value) const;

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

Shader::Shader(const char *vs_path, const char *ps_path) {
  std::string vs_src_str(ReadFromFile(vs_path));
  std::string ps_src_str(ReadFromFile(ps_path));
  const char *vs_src = vs_src_str.c_str();
  const char *ps_src = ps_src_str.c_str();

  unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
  unsigned int ps = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(vs, 1, &vs_src, nullptr);
  glShaderSource(ps, 1, &ps_src, nullptr);

  CompileShader(vs);
  CompileShader(ps);

  id_ = glCreateProgram();
  glAttachShader(id_, vs);
  glAttachShader(id_, ps);
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
  glDeleteShader(ps);
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

Shader::~Shader() { glDeleteProgram(id_); }
} // end namespace xac
