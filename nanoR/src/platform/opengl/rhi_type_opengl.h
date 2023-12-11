#pragma once
#include "glad/glad.h"
#include "render/rhi_type.h"

namespace nanoR {

struct RHIBufferOpenGL : public RHIBuffer {
  ~RHIBufferOpenGL() override {
    if (id != 0) {
      glDeleteBuffers(1, &id);
      id = 0;
    }
  }

  GLuint id;
};

struct RHIBufferCreateInfoOpenGL : public RHIBufferCreateInfo {
  GLsizeiptr size;
  const void *data;
  GLbitfield flags = 0;
};

struct RHIVertexArrayOpenGL : public RHIVertexArray {
  ~RHIVertexArrayOpenGL() override {
    if (id != 0) {
      glDeleteVertexArrays(1, &id);
      id = 0;
    }
  }

  GLuint id;
  GLsizei count;  // index size
};

struct RHIBindVertexBufferInfoOpenGL : public RHIBindVertexBufferInfo {
  struct VertexFormat {
    GLuint attr_index;
    GLint attr_size;
    GLuint reletive_offset;  // where to start interpereting the attr
  };
  GLuint bind_index;
  GLenum type;
  GLboolean normalized;
  GLintptr offset;  // where to start interpereting vbo
  GLsizei stride;
  std::vector<VertexFormat> vertex_format;
};

struct RHIBindIndexBufferInfoOpenGL : public RHIBindIndexBufferInfo {
  GLsizei count;
};

struct RHIShaderModuleCreateInfoOpenGL : public RHIShaderModuleCreateInfo {
  GLenum type;
  const GLchar *src;
  GLint count = 1;
  GLint *length = nullptr;
};

struct RHIShaderModuleOpenGL : public RHIShaderModule {
  ~RHIShaderModuleOpenGL() override {
    glDeleteShader(id);
  }

  GLuint id;
};

struct RHIShaderProgramCreateInfoOpenGL : public RHIShaderProgramCreateInfo {
  std::vector<std::shared_ptr<RHIShaderModule>> shaders;
};

struct RHIShaderProgramOpenGL : public RHIShaderProgram {
  ~RHIShaderProgramOpenGL() override {
    glDeleteProgram(id);
  }

  GLuint id;
};

}  // namespace nanoR