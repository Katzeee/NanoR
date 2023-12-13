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

  template <typename T>
  void SetValue(std::string_view name, T &&value) const {
    glUseProgram(id);
    if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
      glUniform1i(glGetUniformLocation(id, name.data()), static_cast<int>(value));
    } else if constexpr (std::is_same_v<std::decay_t<T>, int>) {
      glUniform1i(glGetUniformLocation(id, name.data()), value);
    } else if constexpr (std::is_same_v<std::decay_t<T>, float>) {
      glUniform1f(glGetUniformLocation(id, name.data()), value);
    } else if constexpr (std::is_same_v<std::decay_t<T>, glm::mat4>) {
      glUniformMatrix4fv(glGetUniformLocation(id, name.data()), 1, GL_FALSE, glm::value_ptr(value));
    } else if constexpr (std::is_same_v<std::decay_t<T>, glm::vec3>) {
      glUniform3fv(glGetUniformLocation(id, name.data()), 1, glm::value_ptr(value));
    } else if constexpr (std::is_same_v<std::decay_t<T>, glm::vec4>) {
      glUniform3fv(glGetUniformLocation(id, name.data()), 1, glm::value_ptr(value));
    }
    glUseProgram(0);
  }

  GLuint id;
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
  GLuint id;
};

struct RHIAttachColorAttachmentInfoOpenGL : public RHIAttachColorAttachmentInfo {
  GLint level;
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

}  // namespace nanoR