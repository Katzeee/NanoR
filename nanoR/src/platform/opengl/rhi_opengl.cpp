#include "rhi_opengl.h"

#include "nanorpch.h"
#include "rhi_type_opengl.h"

namespace nanoR {

bool RHIOpenGL::OpenGLCheckError() {
  if (auto res = glGetError(); res != GL_NO_ERROR) {
    LOG_FATAL("OpenGL API error {}\n", res);
    throw std::runtime_error("OpenGL API error");
  }
  return true;
}

bool RHIOpenGL::CreateBuffer(const RHIBufferCreateInfo &buffer_create_info, std::shared_ptr<RHIBuffer> &buffer) {
  auto buffer_opengl = new RHIBufferOpenGL{};
  glCreateBuffers(1, &buffer_opengl->id);
  const auto &[size, data, flags] = dynamic_cast<const RHIBufferCreateInfoOpenGL &>(buffer_create_info);
  glNamedBufferStorage(buffer_opengl->id, size, data, flags);
  buffer.reset(buffer_opengl);
  return OpenGLCheckError();
}

bool RHIOpenGL::CreateVertexArray(std::shared_ptr<RHIVertexArray> &vertex_array) {
  auto vertex_array_opengl = new RHIVertexArrayOpenGL{};
  glCreateVertexArrays(1, &vertex_array_opengl->id);
  vertex_array.reset(vertex_array_opengl);
  return OpenGLCheckError();
}

bool RHIOpenGL::BindVertexBuffer(const RHIBindVertexBufferInfo &bind_vertex_buffer_info,
                                 std::shared_ptr<RHIVertexArray> vertex_array,
                                 std::shared_ptr<RHIBuffer> vertex_buffer) {
  auto *vertex_array_opengl = dynamic_cast<RHIVertexArrayOpenGL *>(vertex_array.get());
  auto *buffer_opengl = dynamic_cast<RHIBufferOpenGL *>(vertex_buffer.get());
  const auto &[bind_index, attr_index, attr_size, type, normalized, offset, stride] =
      dynamic_cast<const RHIBindVertexBufferInfoOpenGL &>(bind_vertex_buffer_info);
  // Enable the attribute.
  glEnableVertexArrayAttrib(vertex_array_opengl->id, attr_index);
  // Tell OpenGL what the format of the attribute is.
  glVertexArrayAttribFormat(vertex_array_opengl->id, attr_index, attr_size, type, normalized, offset);
  // Bind it to the vertex array - offset , stride
  glVertexArrayVertexBuffer(vertex_array_opengl->id, bind_index, buffer_opengl->id, offset, stride);
  // Tell OpenGL which vertex buffer binding to use for this attribute.
  glVertexArrayAttribBinding(vertex_array_opengl->id, attr_index, bind_index);
  return OpenGLCheckError();
}
bool RHIOpenGL::BindIndexBuffer(const RHIBindIndexBufferInfo &bind_index_buffer_info,
                                std::shared_ptr<RHIVertexArray> vertex_array, std::shared_ptr<RHIBuffer> index_buffer) {
  auto *vertex_array_opengl = dynamic_cast<RHIVertexArrayOpenGL *>(vertex_array.get());
  auto *buffer_opengl = dynamic_cast<RHIBufferOpenGL *>(index_buffer.get());
  const auto &[count] = dynamic_cast<const RHIBindIndexBufferInfoOpenGL &>(bind_index_buffer_info);
  vertex_array_opengl->count = count;
  glVertexArrayElementBuffer(vertex_array_opengl->id, buffer_opengl->id);
  return OpenGLCheckError();
}

bool RHIOpenGL::CreateShaderModule(const RHIShaderModuleCreateInfo &shader_module_create_info,
                                   std::shared_ptr<RHIShaderModule> &shader_module) {
  auto shader_module_opengl = new RHIShaderModuleOpenGL{};
  // const auto &shader_module_create_info_opengl
  const auto &[type, src, count, length] =
      dynamic_cast<const RHIShaderModuleCreateInfoOpenGL &>(shader_module_create_info);
  shader_module_opengl->id = glCreateShader(type);
  glShaderSource(shader_module_opengl->id, count, &src, length);
  glCompileShader(shader_module_opengl->id);
  int success;
  char info[512];
  glGetShaderiv(shader_module_opengl->id, GL_COMPILE_STATUS, &success);
  if (!static_cast<bool>(success)) {
    glGetShaderInfoLog(shader_module_opengl->id, 512, nullptr, info);
    LOG_FATAL("ERROR::SHADER::VERTEX::COMPILATION_FAILED: {}", info);
  }
  shader_module.reset(shader_module_opengl);
  return OpenGLCheckError();
}

bool RHIOpenGL::CreateShaderProgram(const RHIShaderProgramCreateInfo &shader_program_create_info,
                                    std::shared_ptr<RHIShaderProgram> &shader_program) {
  const auto &[shaders] = dynamic_cast<const RHIShaderProgramCreateInfoOpenGL &>(shader_program_create_info);
  auto shader_program_opengl = new RHIShaderProgramOpenGL{};
  shader_program_opengl->id = glCreateProgram();
  for (auto &&shader : shaders) {
    glAttachShader(shader_program_opengl->id, dynamic_cast<RHIShaderModuleOpenGL *>(shader.get())->id);
  }
  glLinkProgram(shader_program_opengl->id);
  int success;
  char info[512];
  glGetProgramiv(shader_program_opengl->id, GL_LINK_STATUS, &success);
  if (!static_cast<bool>(success)) {
    glGetProgramInfoLog(shader_program_opengl->id, 512, nullptr, info);
    LOG_FATAL("ERROR::SHADER::PROGRAM::LINKING_FAILED: {}", info);
  }
  shader_program.reset(shader_program_opengl);
  return OpenGLCheckError();
}

bool RHIOpenGL::Draw(std::shared_ptr<RHIVertexArray> vertex_array, std::shared_ptr<RHIShaderProgram> shader_program) {
  glUseProgram(dynamic_cast<RHIShaderProgramOpenGL *>(shader_program.get())->id);
  auto *vertex_array_opengl = dynamic_cast<RHIVertexArrayOpenGL *>(vertex_array.get());
  glBindVertexArray(vertex_array_opengl->id);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 2);
  glDrawElements(GL_TRIANGLES, vertex_array_opengl->count, GL_UNSIGNED_INT, nullptr);
  // glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  glUseProgram(0);
  return OpenGLCheckError();
}

}  // namespace nanoR