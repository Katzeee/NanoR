#pragma once
#include "glad/glad.h"
#include "render/rhi_resource.h"
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

struct RHIVertexArrayOpenGL {
  ~RHIVertexArrayOpenGL() {
    if (id != 0) {
      glDeleteVertexArrays(1, &id);
      id = 0;
    }
  }

  GLuint id;
  GLsizei count; // index size
};

struct RHIBindVertexBufferInfoOpenGL : public RHIBindVertexBufferInfo {
  struct VertexFormat {
    GLuint attr_index;
    GLint attr_size;
    GLuint reletive_offset; // where to start interpereting the attr
  };
  GLuint bind_index;
  GLenum type;
  GLboolean normalized;
  GLintptr offset; // where to start interpereting vbo
  GLsizei stride;
  std::vector<VertexFormat> vertex_format;
};

struct RHIBindIndexBufferInfoOpenGL : public RHIBindIndexBufferInfo {
  GLsizei count;
};

struct RHIShaderModuleCreateInfoOpenGL : public RHIShaderModuleCreateInfo {
  GLenum type;
  const char *file;
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

// TODO: remove
struct RHIBindUniformBufferInfoOpenGL {
  GLenum target;
  GLuint index; // binding index
};

struct RHIFramebufferCreateInfoOpenGL : public RHIFramebufferCreateInfo {};

struct RHITextureCreateInfoOpenGL : public RHITextureCreateInfo {
  struct Parameteri {
    GLenum pname;
    GLint param;
  };
  GLenum target;
  GLsizei levels;
  GLenum internal_format;
  GLsizei width;
  GLsizei height;
  GLenum format;
  GLenum type;
  void *data;
  std::vector<Parameteri> parameteri;
};

struct RHITextureOpenGL : public RHITexture {
  ~RHITextureOpenGL() {
    glDeleteTextures(1, &id);
  }
  GLenum target;
  GLuint id;
};

struct RHIAttachTextureInfoOpenGL : public RHIAttachTextureInfo {
  GLint level;
  GLenum attachment;
};

struct RHIAttachDepthAttachmentInfoOpenGL : public RHIAttachDepthAttachmentInfo {};
struct RHIAttachStencilAttachmentInfoOpenGL : public RHIAttachStencilAttachmentInfo {};
struct RHIAttachDepthStencilAttachmentInfoOpenGL : public RHIAttachDepthStencilAttachmentInfo {};

struct RHIFramebufferOpenGL : public RHIFramebuffer {
  ~RHIFramebufferOpenGL() override {
    glDeleteFramebuffers(1, &id);
  }
  GLuint id;
};

} // namespace nanoR