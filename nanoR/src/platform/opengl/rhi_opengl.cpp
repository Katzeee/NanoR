#include "rhi_opengl.h"

#include "nanorpch.h"
#include "rhi_type_opengl.h"

namespace nanoR {

bool RHIOpenGL::OpenGLCheckError() {
  if (auto res = glGetError(); res != GL_NO_ERROR) {
    LOG_FATAL("OpenGL API error {}\n", res);
    // throw std::runtime_error("OpenGL API error");
  }
  return true;
}

bool RHIOpenGL::CreateBuffer(const RHIBufferCreateInfo &buffer_create_info, std::shared_ptr<RHIBuffer> &buffer) {
  auto buffer_opengl = new RHIBufferOpenGL();
  glCreateBuffers(1, &buffer_opengl->id);
  const auto &buffer_create_info_opengl = dynamic_cast<const RHIBufferCreateInfoOpenGL &>(buffer_create_info);
  glNamedBufferStorage(buffer_opengl->id, buffer_create_info_opengl.size, buffer_create_info_opengl.data,
                       buffer_create_info_opengl.flags);
  buffer.reset(buffer_opengl);
  return OpenGLCheckError();
}

bool RHIOpenGL::CreateVertexArray(std::shared_ptr<RHIVertexArray> &vertex_array) {
  auto vertex_array_opengl = new RHIVertexArrayOpenGL();
  glCreateVertexArrays(1, &vertex_array_opengl->id);
  vertex_array.reset(vertex_array_opengl);
  return OpenGLCheckError();
}

bool RHIOpenGL::BindVertexBuffer(const RHIBindVertexBufferInfo &bind_vertex_buffer_info,
                                 std::shared_ptr<RHIVertexArray> vertex_array, std::shared_ptr<RHIBuffer> buffer) {
  auto *vertex_array_opengl = dynamic_cast<RHIVertexArrayOpenGL *>(vertex_array.get());
  auto *buffer_opengl = dynamic_cast<RHIBufferOpenGL *>(buffer.get());
  const auto &bind_vertex_buffer_info_opengl =
      dynamic_cast<const RHIBindVertexBufferInfoOpenGL &>(bind_vertex_buffer_info);
  // Enable the attribute.
  glEnableVertexArrayAttrib(vertex_array_opengl->id, bind_vertex_buffer_info_opengl.attr_index);
  // Tell OpenGL what the format of the attribute is.
  glVertexArrayAttribFormat(vertex_array_opengl->id, bind_vertex_buffer_info_opengl.attr_index,
                            bind_vertex_buffer_info_opengl.attr_size, bind_vertex_buffer_info_opengl.type,
                            bind_vertex_buffer_info_opengl.normalized, bind_vertex_buffer_info_opengl.offset);
  // Bind it to the vertex array - offset , stride
  glVertexArrayVertexBuffer(vertex_array_opengl->id, bind_vertex_buffer_info_opengl.bind_index, buffer_opengl->id,
                            bind_vertex_buffer_info_opengl.offset, bind_vertex_buffer_info_opengl.stride);
  // Tell OpenGL which vertex buffer binding to use for this attribute.
  glVertexArrayAttribBinding(vertex_array_opengl->id, bind_vertex_buffer_info_opengl.attr_index,
                             bind_vertex_buffer_info_opengl.bind_index);
  return OpenGLCheckError();
}
bool RHIOpenGL::BindIndexBuffer(const RHIBindIndexBufferInfo &bind_index_buffer_info,
                                std::shared_ptr<RHIVertexArray> vertex_array, std::shared_ptr<RHIBuffer> buffer) {
  auto *vertex_array_opengl = dynamic_cast<RHIVertexArrayOpenGL *>(vertex_array.get());
  auto *buffer_opengl = dynamic_cast<RHIBufferOpenGL *>(buffer.get());
  const auto &bind_index_buffer_info_opengl =
      dynamic_cast<const RHIBindIndexBufferInfoOpenGL &>(bind_index_buffer_info);
  vertex_array_opengl->count = bind_index_buffer_info_opengl.count;
  glVertexArrayElementBuffer(vertex_array_opengl->id, buffer_opengl->id);
  return OpenGLCheckError();
}

bool RHIOpenGL::Draw(std::shared_ptr<RHIVertexArray> vertex_array) {
  auto *vertex_array_opengl = dynamic_cast<RHIVertexArrayOpenGL *>(vertex_array.get());
  glBindVertexArray(vertex_array_opengl->id);
  glDrawElements(GL_TRIANGLES, vertex_array_opengl->count, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
  return OpenGLCheckError();
}

}  // namespace nanoR