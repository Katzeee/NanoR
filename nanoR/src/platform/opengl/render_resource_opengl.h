#pragma once
#include "global/global_context.h"
#include "nanorpch.h"
#include "render/resource_manager.h"
#include "rhi_opengl.h"

namespace nanoR {

// struct OpenGLMesh {
//   std::shared_ptr<RHIBuffer> vbo;
//   std::shared_ptr<RHIBuffer> ebo;
//   std::shared_ptr<RHIVertexArray> vao;
// };

static auto CreateMesh(const MeshData& mesh_data) -> std::shared_ptr<RHIVertexArray> {
  auto& rhi = GlobalContext::Instance().rhi;
  std::shared_ptr<RHIVertexArray> vao;
  std::shared_ptr<RHIBuffer> vbo;
  std::shared_ptr<RHIBuffer> ebo;
  // Create vertex array
  rhi->CreateVertexArray(vao);
  // Create vertex buffer
  auto buffer_create_info = nanoR::RHIBufferCreateInfoOpenGL{};
  buffer_create_info.size = mesh_data.vertices.size() * sizeof(MeshData::Vertex);
  buffer_create_info.data = mesh_data.vertices.data();
  buffer_create_info.flags = 0;
  rhi->CreateBuffer(buffer_create_info, vbo);
  // Create index buffer
  buffer_create_info.size = mesh_data.indices.size() * sizeof(uint32_t);
  buffer_create_info.data = mesh_data.indices.data();
  rhi->CreateBuffer(buffer_create_info, ebo);
  // Bind vertex buffer
  auto bind_vertex_buffer_info = nanoR::RHIBindVertexBufferInfoOpenGL{};
  bind_vertex_buffer_info.bind_index = 0;
  bind_vertex_buffer_info.stride = sizeof(MeshData::Vertex);
  bind_vertex_buffer_info.normalized = GL_FALSE;
  bind_vertex_buffer_info.type = GL_FLOAT;
  bind_vertex_buffer_info.offset = 0;
  bind_vertex_buffer_info.vertex_format.push_back({0, 3, offsetof(MeshData::Vertex, position)});
  bind_vertex_buffer_info.vertex_format.push_back({1, 3, offsetof(MeshData::Vertex, normal)});
  bind_vertex_buffer_info.vertex_format.push_back({2, 2, offsetof(MeshData::Vertex, texcoord)});
  bind_vertex_buffer_info.vertex_format.push_back({3, 3, offsetof(MeshData::Vertex, tangent)});
  rhi->BindVertexBuffer(bind_vertex_buffer_info, vao, vbo);
  // Bind index buffer
  auto bind_index_buffer_info = nanoR::RHIBindIndexBufferInfoOpenGL{};
  bind_index_buffer_info.count = mesh_data.indices.size();
  rhi->BindIndexBuffer(bind_index_buffer_info, vao, ebo);

  return vao;
}

// static auto CreateShaderProgram(RHI* rhi, char const* vs_path, char const* fs_path) -> {}

}  // namespace nanoR