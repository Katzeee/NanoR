#pragma once
#include <glad/glad.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <set>
#include <sstream>

namespace xac {

class Shader {
 public:
  Shader() = delete;
  Shader(const char *vs_path, const char *fs_path);
  ~Shader();
  void Use() const;
  void SetBool(std::string_view name, bool value) const;
  void SetInt(std::string_view name, int value) const;
  void SetFloat(std::string_view name, float value) const;
  void SetMat4(std::string_view name, const glm::mat4 &mat) const;
  void SetVec3(std::string_view name, const glm::vec3 &vec) const;
  void SetVec4(std::string_view name, const glm::vec4 &vec) const;
  void AddDefine(const std::string &name);
  void DeleteDefine(const std::string &name);
  void ClearDefine();
  void CompileShaders();
  [[nodiscard]] auto GetId() const -> const unsigned int & { return id_; }

 private:
  static auto ReadFromFile(const char *file_path) -> std::string;
  template <int type>
  auto CompileShader(std::string shader_str) -> unsigned int;

  unsigned int id_ = 0;
  std::string vs_str_;
  std::string fs_str_;
  std::set<std::string> define_consts_;
};

}  // end namespace xac
