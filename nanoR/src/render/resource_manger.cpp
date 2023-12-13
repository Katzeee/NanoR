#include "nanorpch.h"
#include "platform/opengl/rhi_type_opengl.h"
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

// TODO: read from file
auto ResourceManager::GetCubeMeshData() -> MeshData {
  MeshData mesh_data;
  mesh_data.vertices.resize(8);
  mesh_data.vertices[0].position = {0.5, 0.5, -0.5};
  mesh_data.vertices[1].position = {0.5, -0.5, -0.5};
  mesh_data.vertices[2].position = {-0.5, -0.5, -0.5};
  mesh_data.vertices[3].position = {-0.5, 0.5, -0.5};
  mesh_data.vertices[4].position = {0.5, 0.5, 0.5};
  mesh_data.vertices[5].position = {0.5, -0.5, 0.5};
  mesh_data.vertices[6].position = {-0.5, -0.5, 0.5};
  mesh_data.vertices[7].position = {-0.5, 0.5, 0.5};
  mesh_data.indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 0, 1, 5, 0, 5, 4,
                       2, 3, 7, 2, 7, 6, 0, 3, 7, 0, 7, 4, 1, 2, 6, 1, 6, 5};
  return mesh_data;
}

// TODO: not create like this
auto ResourceManager::GetUnlitShader(RHI *rhi) -> std::shared_ptr<RHIShaderProgram> {
  ShaderData shader_data =
      ResourceManager::LoadShaderData("../nanoR/shader/common.vert.glsl", "../nanoR/shader/unlit.frag.glsl");
  std::shared_ptr<RHIShaderModule> vert_shader;
  std::shared_ptr<RHIShaderModule> frag_shader;
  std::shared_ptr<RHIShaderProgram> shader_program;
  auto shader_module_create_info = RHIShaderModuleCreateInfoOpenGL{};
  shader_module_create_info.type = GL_VERTEX_SHADER;
  shader_module_create_info.src = shader_data.vs_src.c_str();
  rhi->CreateShaderModule(shader_module_create_info, vert_shader);
  shader_module_create_info.type = GL_FRAGMENT_SHADER;
  shader_module_create_info.src = shader_data.fs_src.c_str();
  rhi->CreateShaderModule(shader_module_create_info, frag_shader);
  auto shader_program_create_info = RHIShaderProgramCreateInfoOpenGL{};
  shader_program_create_info.shaders.push_back(vert_shader);
  shader_program_create_info.shaders.push_back(frag_shader);
  rhi->CreateShaderProgram(shader_program_create_info, shader_program);
  return shader_program;
}
}  // namespace nanoR