#include "global/global_context.h"
#include "nanorpch.h"
#include "render/rhi_type_opengl.h"
#include "resource_manager.h"

namespace nanoR {

ResourceManager::ResourceManager() {
  rhi_ = GlobalContext::Instance().rhi;
  Init();
}

auto ResourceManager::Init() -> void {
  // LoadShader("unlit", "../nanoR/shader/common.vert.glsl", "../nanoR/shader/unlit.frag.glsl");
  // LoadShader("lit", "../nanoR/shader/common.vert.glsl", "../nanoR/shader/lit.frag.glsl");
  // LoadShader("ui", "../nanoR/shader/ui.vert.glsl", "../nanoR/shader/unlit.frag.glsl");
  // LoadShader("pbr", "../nanoR/shader/common.vert.glsl", "../nanoR/shader/pbr.frag.glsl");
  LoadShader("ui", "../00-nanoR/shader/common.vert.glsl", "../00-nanoR/shader/test.frag.glsl");
  LoadShader("lit", "../00-nanoR/shader/common.vert.glsl", "../00-nanoR/shader/test.frag.glsl");
  LoadTexture("white", "../resources/textures/white.png");
  LoadTexture("point-light", "../resources/textures/point-light.png");
}

auto ResourceManager::LoadShader(std::string_view name, char const *vs_path, char const *fs_path) -> void {
  ShaderData shader_data;
  shader_data.vs_src = ReadTextFromFile(vs_path);
  shader_data.fs_src = ReadTextFromFile(fs_path);
  std::shared_ptr<RHIShaderModule> vert_shader;
  std::shared_ptr<RHIShaderModule> frag_shader;
  std::shared_ptr<RHIShaderProgram> shader_program;
  // TODO: no opengl
  auto shader_module_create_info = RHIShaderModuleCreateInfoOpenGL{};
  shader_module_create_info.type = GL_VERTEX_SHADER;
  shader_module_create_info.file = vs_path;
  shader_module_create_info.src = shader_data.vs_src.c_str();
  rhi_->CreateShaderModule(shader_module_create_info, vert_shader);
  shader_module_create_info.type = GL_FRAGMENT_SHADER;
  shader_module_create_info.file = fs_path;
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
}

auto ResourceManager::GetTexture(std::string_view name) -> std::shared_ptr<RHITexture> {
  if (auto it = textures_.find(name.data()); it != textures_.end()) {
    return it->second;
  }
  LOG_ERROR("No texture named {}\n", name);
  throw std::runtime_error("cannot find texture");
}

auto ResourceManager::ReadTextFromFile(char const *file_path) -> std::string {
  std::ifstream fs;
  std::stringstream ss;
  fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    fs.open(file_path);
    ss << fs.rdbuf();
    fs.close();
  } catch (std::exception &e) {
    LOG_ERROR("ERROR::SHADER::READ_FAILED: {}\n", file_path);
    exit(3);
  }
  return ss.str();
}

auto ResourceManager::LoadTexture(std::string_view name, std::string_view file_path) -> void {
  static std::unordered_map<int, GLenum> ch_map{{1, GL_RED}, {3, GL_RGB}, {4, GL_RGBA}};
  int width, height, nchs;
  std::shared_ptr<RHITexture> texture;
  auto *image_data = stbi_load(file_path.data(), &width, &height, &nchs, 0);
  // TODO: no opengl
  RHITextureCreateInfoOpenGL texture_create_info;
  texture_create_info.target = GL_TEXTURE_2D;
  texture_create_info.levels = 1;
  texture_create_info.internal_format = GL_RGBA16;
  texture_create_info.width = width;
  texture_create_info.height = height;
  texture_create_info.format = ch_map[nchs];
  texture_create_info.type = GL_UNSIGNED_BYTE;
  texture_create_info.data = image_data;
  texture_create_info.parameteri.emplace_back(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  texture_create_info.parameteri.emplace_back(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  texture_create_info.parameteri.emplace_back(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  texture_create_info.parameteri.emplace_back(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  rhi_->CreateTexture(texture_create_info, texture);
  stbi_image_free(image_data);
  textures_[name.data()] = texture;
  LOG_TRACE("Load texture: {}, channels: {}\n", file_path, nchs);
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

// auto ResourceManager::GetSpereMeshData() -> MeshData {
//   unsigned int sphereVAO = 0;
//   glGenVertexArrays(1, &sphereVAO);

//   unsigned int vbo, ebo;
//   glGenBuffers(1, &vbo);
//   glGenBuffers(1, &ebo);

//   std::vector<glm::vec3> positions;
//   std::vector<glm::vec3> normals;
//   std::vector<glm::vec2> uv;
//   std::vector<unsigned int> indices;

//   const unsigned int X_SEGMENTS = 64;
//   const unsigned int Y_SEGMENTS = 64;
//   const float PI = 3.14159265359f;
//   for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
//     for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
//       float xSegment = (float)x / (float)X_SEGMENTS;
//       float ySegment = (float)y / (float)Y_SEGMENTS;
//       float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
//       float yPos = std::cos(ySegment * PI);
//       float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

//       positions.push_back(glm::vec3(xPos, yPos, zPos));
//       normals.push_back(glm::vec3(xPos, yPos, zPos));
//       uv.push_back(glm::vec2(xSegment, ySegment));
//     }
//   }

//   bool oddRow = false;
//   for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
//     if (!oddRow)  // even rows: y == 0, y == 2; and so on
//     {
//       for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
//         indices.push_back(y * (X_SEGMENTS + 1) + x);
//         indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
//       }
//     } else {
//       for (int x = X_SEGMENTS; x >= 0; --x) {
//         indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
//         indices.push_back(y * (X_SEGMENTS + 1) + x);
//       }
//     }
//     oddRow = !oddRow;
//   }
//   indexCount = static_cast<unsigned int>(indices.size());

//   std::vector<float> data;
//   for (unsigned int i = 0; i < positions.size(); ++i) {
//     data.push_back(positions[i].x);
//     data.push_back(positions[i].y);
//     data.push_back(positions[i].z);
//     if (normals.size() > 0) {
//       data.push_back(normals[i].x);
//       data.push_back(normals[i].y);
//       data.push_back(normals[i].z);
//     }
//     if (uv.size() > 0) {
//       data.push_back(uv[i].x);
//       data.push_back(uv[i].y);
//     }
//   }
//   glBindVertexArray(sphereVAO);
//   glBindBuffer(GL_ARRAY_BUFFER, vbo);
//   glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
//   unsigned int stride = (3 + 3 + 2) * sizeof(float);
//   glEnableVertexAttribArray(0);
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
//   glEnableVertexAttribArray(1);
//   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
//   glEnableVertexAttribArray(2);
//   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));

//   return sphereVAO;
// }

} // namespace nanoR