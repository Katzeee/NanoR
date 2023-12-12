#include "nanorpch.h"
#include "resource_manager.h"

namespace nanoR {

auto ResourceManager::LoadShaderData(char const *vs_path, char const *fs_path) -> ShaderData {
  ShaderData shader_data;
  shader_data.vs_src = ReadFromFile(vs_path);
  shader_data.fs_src = ReadFromFile(fs_path);
  return shader_data;
}

auto ResourceManager::ReadFromFile(char const *file_path) -> std::string {
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
}  // namespace nanoR