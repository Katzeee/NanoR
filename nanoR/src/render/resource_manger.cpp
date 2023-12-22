#include "global/global_context.h"
#include "nanorpch.h"
#include "platform/opengl/rhi_type_opengl.h"
#include "resource_manager.h"

namespace nanoR {

ResourceManager::ResourceManager() {
  rhi_ = GlobalContext::Instance().rhi;
  Init();
}

auto ResourceManager::Init() -> void {
  LoadShader("unlit", "../nanoR/shader/common.vert.glsl", "../nanoR/shader/unlit.frag.glsl");
  LoadShader("lit", "../nanoR/shader/common.vert.glsl", "../nanoR/shader/lit.frag.glsl");
  LoadShader("ui", "../nanoR/shader/ui.vert.glsl", "../nanoR/shader/unlit.frag.glsl");
}

auto ResourceManager::LoadShader(std::string_view name, char const *vs_path, char const *fs_path) -> void {
  ShaderData shader_data;
  shader_data.vs_src = ReadFromFile(vs_path);
  shader_data.fs_src = ReadFromFile(fs_path);
  std::shared_ptr<RHIShaderModule> vert_shader;
  std::shared_ptr<RHIShaderModule> frag_shader;
  std::shared_ptr<RHIShaderProgram> shader_program;
  auto shader_module_create_info = RHIShaderModuleCreateInfoOpenGL{};
  shader_module_create_info.type = GL_VERTEX_SHADER;
  shader_module_create_info.src = shader_data.vs_src.c_str();
  rhi_->CreateShaderModule(shader_module_create_info, vert_shader);
  shader_module_create_info.type = GL_FRAGMENT_SHADER;
  shader_module_create_info.src = shader_data.fs_src.c_str();
  rhi_->CreateShaderModule(shader_module_create_info, frag_shader);
  auto shader_program_create_info = RHIShaderProgramCreateInfoOpenGL{};
  shader_program_create_info.shaders.push_back(vert_shader);
  shader_program_create_info.shaders.push_back(frag_shader);
  rhi_->CreateShaderProgram(shader_program_create_info, shader_program);
  shaders_[name.data()] = shader_program;
  LOG_TRACE("Load shader {}, from {}, {}\n", name, vs_path, fs_path);
}

auto ResourceManager::GetShader(std::string_view name) -> std::shared_ptr<RHIShaderProgram> {
  if (auto it = shaders_.find(name.data()); it != shaders_.end()) {
    return it->second;
  }
  LOG_FATAL("No shader named {}\n", name);
  throw std::runtime_error("cannot find shader");
  // return shaders_;
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

auto ResourceManager::LoadTextureFromFile(std::string_view file_path) -> unsigned int {
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
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    stbi_image_free(image_data);
    LOG_TRACE("Load texture: {}, channels: {}\n", file_path, nchs);
  } else {
    LOG_ERROR("Load texture: {}\n", file_path);
  }
  return texture_id;
}

auto ResourceManager::GetQuadMeshData() -> MeshData {
  MeshData mesh_data;
  mesh_data.vertices.resize(4);
  mesh_data.vertices[0].position = {-1, -1, 0};
  mesh_data.vertices[0].normal = {0, 0, 1};
  mesh_data.vertices[0].texcoord = {0, 0};
  mesh_data.vertices[1].position = {1, -1, 0};
  mesh_data.vertices[1].normal = {0, 0, 1};
  mesh_data.vertices[1].texcoord = {1, 0};
  mesh_data.vertices[2].position = {-1, 1, 0};
  mesh_data.vertices[2].normal = {0, 0, 1};
  mesh_data.vertices[2].texcoord = {0, 1};
  mesh_data.vertices[3].position = {1, 1, 0};
  mesh_data.vertices[3].normal = {0, 0, 1};
  mesh_data.vertices[3].texcoord = {1, 1};
  mesh_data.indices = {0, 1, 2, 1, 2, 3};
  return mesh_data;
}

}  // namespace nanoR