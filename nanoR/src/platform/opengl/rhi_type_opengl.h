#pragma once
#include "glad/glad.h"
#include "render/rhi_type.h"

namespace nanoR {

struct RHIBufferOpenGL : public RHIBuffer {
  ~RHIBufferOpenGL() {
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
  GLbitfield flags;
};

struct RHIVertexArrayOpenGL : public RHIVertexArray {
  ~RHIVertexArrayOpenGL() {
    if (id != 0) {
      glDeleteVertexArrays(1, &id);
      id = 0;
    }
  }

  GLuint id;
  GLsizei count;  // index size
};

struct RHIBindVertexBufferInfoOpenGL : public RHIBindVertexBufferInfo {
  GLuint bind_index;
  GLuint attr_index;
  GLuint attr_size;
  GLenum type;
  GLboolean normalized;
  GLintptr offset;
  GLsizei stride;
};

struct RHIBindIndexBufferInfoOpenGL : public RHIBindIndexBufferInfo {
  GLsizei count;
};

}  // namespace nanoR