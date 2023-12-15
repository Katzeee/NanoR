#include "shader/shader.hpp"
namespace xac {

auto Shader::ReadFromFile(const char *file_path) -> std::string {
  std::ifstream fs;
  std::stringstream ss;
  fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    fs.open(file_path);
    ss << fs.rdbuf();
    fs.close();
  } catch (std::exception &e) {
    std::cout << "ERROR::SHADER::READ_FAILED\n" << file_path << std::endl;
    exit(3);
  }
  return ss.str();
}

template <int type>
auto Shader::CompileShader(std::string shader_str) -> unsigned int {
  for (auto &&group : define_consts_) {
    for (auto &&dc : group.second) {
      shader_str.insert(shader_str.find_first_of('\n'), "\n#define " + dc);
    }
  }
  char const *shader_src = shader_str.c_str();
  unsigned int shader_id = glCreateShader(type);
  glShaderSource(shader_id, 1, &shader_src, nullptr);
  glCompileShader(shader_id);
  int success;
  char info[512];
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!static_cast<bool>(success)) {
    glGetShaderInfoLog(shader_id, 512, nullptr, info);
    std::cout << shader_str << std::endl;
    std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << info << std::endl;
  };
  return shader_id;
}

void Shader::CompileShaders() {
  auto vs = CompileShader<GL_VERTEX_SHADER>(vs_str_);
  auto fs = CompileShader<GL_FRAGMENT_SHADER>(fs_str_);
  if (id_ != 0) {
    glDeleteProgram(id_);
  }
  id_ = glCreateProgram();
  glAttachShader(id_, vs);
  glAttachShader(id_, fs);
  glLinkProgram(id_);

  int success;
  char info[512];
  glGetProgramiv(id_, GL_LINK_STATUS, &success);
  if (!static_cast<bool>(success)) {
    glGetProgramInfoLog(id_, 512, nullptr, info);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info << std::endl;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);
}

Shader::Shader(const char *vs_path, const char *fs_path) {
  vs_str_ = ReadFromFile(vs_path);
  fs_str_ = ReadFromFile(fs_path);
  CompileShaders();
}

void Shader::Use() const {
  glUseProgram(id_);
}

void Shader::SetBool(std::string_view name, bool value) const {
  glUniform1i(glGetUniformLocation(id_, name.data()), static_cast<int>(value));
}

void Shader::SetInt(std::string_view name, int value) const {
  glUniform1i(glGetUniformLocation(id_, name.data()), value);
}
void Shader::SetFloat(std::string_view name, float value) const {
  glUniform1f(glGetUniformLocation(id_, name.data()), value);
}

void Shader::SetMat3(std::string_view name, const glm::mat3 &mat) const {
  glUniformMatrix3fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMat4(std::string_view name, const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetVec3(std::string_view name, const glm::vec3 &vec) const {
  glUniform3f(glGetUniformLocation(id_, name.data()), vec.x, vec.y, vec.z);
}

void Shader::SetVec4(std::string_view name, const glm::vec4 &vec) const {
  glUniform4fv(glGetUniformLocation(id_, name.data()), 1, &vec[0]);
}

void Shader::AddDefine(const std::string &group, const std::string &name) {
  if (define_consts_.contains(group)) {
    define_consts_[group].insert(name);
  } else {
    define_consts_[group] = std::set{name};
  }
}

void Shader::DeleteDefine(const std::string &group, const std::string &name) {
  if (define_consts_.contains(group)) {
    define_consts_[group].erase(name);
  }
}
void Shader::ClearDefineGroup(const std::string &group) {
  if (define_consts_.contains(group)) {
    define_consts_.erase(group);
  }
}

Shader::~Shader() = default;

}  // end namespace xac