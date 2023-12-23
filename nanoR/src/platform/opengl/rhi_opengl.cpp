#include "rhi_opengl.h"

#include "nanorpch.h"
#include "rhi_type_opengl.h"

namespace nanoR {

auto RHIOpenGL::OpenGLCheckError() -> bool {
  if (auto res = glGetError(); res != GL_NO_ERROR) {
    LOG_FATAL("OpenGL API error {}\n", res);
    throw std::runtime_error("OpenGL API error");
  }
  return true;
}

auto RHIOpenGL::CreateBuffer(const RHIBufferCreateInfo &buffer_create_info, std::shared_ptr<RHIBuffer> &buffer)
    -> bool {
  auto *buffer_opengl = new RHIBufferOpenGL{};
  glCreateBuffers(1, &buffer_opengl->id);
  const auto &[size, data, flags] = dynamic_cast<const RHIBufferCreateInfoOpenGL &>(buffer_create_info);
  glNamedBufferStorage(buffer_opengl->id, size, data, flags);
  buffer.reset(buffer_opengl);
  return OpenGLCheckError();
}

bool RHIOpenGL::SetBufferData(const RHISetBufferDataInfo &set_buffer_data_info, RHIBuffer *buffer) {
  auto *buffer_opengl = reinterpret_cast<RHIBufferOpenGL *>(buffer);
  const auto &[offset, size, data] = dynamic_cast<const RHISetBufferDataInfoOpenGL &>(set_buffer_data_info);
  glNamedBufferSubData(buffer_opengl->id, offset, size, data);
  return OpenGLCheckError();
}

auto RHIOpenGL::CreateVertexArray(std::shared_ptr<RHIVertexArray> &vertex_array) -> bool {
  auto *vertex_array_opengl = new RHIVertexArrayOpenGL{};
  glCreateVertexArrays(1, &vertex_array_opengl->id);
  vertex_array.reset(vertex_array_opengl);
  return OpenGLCheckError();
}

auto RHIOpenGL::BindVertexBuffer(
    const RHIBindVertexBufferInfo &bind_vertex_buffer_info, std::shared_ptr<RHIVertexArray> vertex_array,
    std::shared_ptr<RHIBuffer> vertex_buffer
) -> bool {
  auto *vertex_array_opengl = dynamic_cast<RHIVertexArrayOpenGL *>(vertex_array.get());
  auto *buffer_opengl = dynamic_cast<RHIBufferOpenGL *>(vertex_buffer.get());
  const auto &[bind_index, type, normalized, offset, stride, vertex_format] =
      dynamic_cast<const RHIBindVertexBufferInfoOpenGL &>(bind_vertex_buffer_info);
  // Bind vbo to vao via binding index
  glVertexArrayVertexBuffer(vertex_array_opengl->id, bind_index, buffer_opengl->id, offset, stride);
  for (auto &&[attr_index, attr_size, reletive_offset] : vertex_format) {
    // Enable the attribute.
    glEnableVertexArrayAttrib(vertex_array_opengl->id, attr_index);
    // Tell OpenGL what the format of the attribute is.
    glVertexArrayAttribFormat(vertex_array_opengl->id, attr_index, attr_size, type, normalized, reletive_offset);
    // Tell OpenGL which vertex buffer binding to use for this attribute.
    glVertexArrayAttribBinding(vertex_array_opengl->id, attr_index, bind_index);
  }
  return OpenGLCheckError();
}
auto RHIOpenGL::BindIndexBuffer(
    const RHIBindIndexBufferInfo &bind_index_buffer_info, std::shared_ptr<RHIVertexArray> vertex_array,
    std::shared_ptr<RHIBuffer> index_buffer
) -> bool {
  auto *vertex_array_opengl = dynamic_cast<RHIVertexArrayOpenGL *>(vertex_array.get());
  auto *buffer_opengl = dynamic_cast<RHIBufferOpenGL *>(index_buffer.get());
  const auto &[count] = dynamic_cast<const RHIBindIndexBufferInfoOpenGL &>(bind_index_buffer_info);
  vertex_array_opengl->count = count;
  glVertexArrayElementBuffer(vertex_array_opengl->id, buffer_opengl->id);
  return OpenGLCheckError();
}

auto RHIOpenGL::CreateShaderModule(
    const RHIShaderModuleCreateInfo &shader_module_create_info, std::shared_ptr<RHIShaderModule> &shader_module
) -> bool {
  auto *shader_module_opengl = new RHIShaderModuleOpenGL{};
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
    LOG_FATAL("ERROR::SHADER::{}::COMPILATION_FAILED: {}", type, info);
  }
  shader_module.reset(shader_module_opengl);
  return OpenGLCheckError();
}

auto RHIOpenGL::CreateShaderProgram(
    const RHIShaderProgramCreateInfo &shader_program_create_info, std::shared_ptr<RHIShaderProgram> &shader_program
) -> bool {
  const auto &[shaders] = dynamic_cast<const RHIShaderProgramCreateInfoOpenGL &>(shader_program_create_info);
  auto *shader_program_opengl = new RHIShaderProgramOpenGL{};
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
    LOG_FATAL("ERROR::SHADER::PROGRAM::LINKING_FAILED: {}\n", info);
  }
  shader_program.reset(shader_program_opengl);
  return OpenGLCheckError();
}

auto RHIOpenGL::SetShaderUniform(
    RHISetShaderUniformInfo const &set_shader_uniform_info, RHIShaderProgram *shader_program
) -> bool {
  const auto &[uniforms] = set_shader_uniform_info;
  for (auto &&uniform : uniforms) {
    std::visit(
        [&](auto &&value) {
          using T = std::decay_t<decltype(value)>;
          dynamic_cast<RHIShaderProgramOpenGL *>(shader_program)->SetValue<T>(uniform.name, static_cast<T>(value));
        },
        uniform.value
    );
  }
  return OpenGLCheckError();
}

auto RHIOpenGL::BindUniformBuffer(const RHIBindUniformBufferInfo &bind_uniform_buffer_info, RHIBuffer *buffer) -> bool {
  const auto &[target, index] = dynamic_cast<const RHIBindUniformBufferInfoOpenGL &>(bind_uniform_buffer_info);
  auto buffer_opengl = dynamic_cast<RHIBufferOpenGL *>(buffer);
  glBindBufferBase(target, index, buffer_opengl->id);
  return OpenGLCheckError();
}

auto RHIOpenGL::CreateFramebuffer(
    const RHIFramebufferCreateInfo &framebuffer_create_info, std::shared_ptr<RHIFramebuffer> &framebuffer
) -> bool {
  auto *framebuffer_opengl = new RHIFramebufferOpenGL{};
  glCreateFramebuffers(1, &framebuffer_opengl->id);
  framebuffer.reset(framebuffer_opengl);
  return OpenGLCheckError();
}

auto RHIOpenGL::CreateTexture(const RHITextureCreateInfo &texture_create_info, std::shared_ptr<RHITexture> &texture)
    -> bool {
  auto *texture_opengl = new RHITextureOpenGL();
  const auto &[target, levels, internal_format, width, height, format, type, data, parameteri] =
      dynamic_cast<const RHITextureCreateInfoOpenGL &>(texture_create_info);
  glCreateTextures(target, 1, &texture_opengl->id);
  texture_opengl->target = target;
  if (target == GL_TEXTURE_2D) {
    glTextureStorage2D(texture_opengl->id, levels, internal_format, width, height);
    if (data) {
      glTextureSubImage2D(texture_opengl->id, 0, 0, 0, width, height, format, type, data);
    }
    glGenerateTextureMipmap(texture_opengl->id);
  } else {
    throw std::runtime_error("not implement");
  }
  for (auto &&[pname, param] : parameteri) {
    glTextureParameteri(texture_opengl->id, pname, param);
  }
  texture.reset(texture_opengl);
  return OpenGLCheckError();
}

auto RHIOpenGL::AttachTexture(
    const RHIAttachTextureInfo &attach_color_attachment_info, RHIFramebuffer const *framebuffer,
    RHITexture const *texture
) -> bool {
  const auto &[fbo] = *dynamic_cast<RHIFramebufferOpenGL const *>(framebuffer);
  const auto &[level, attachment] = dynamic_cast<const RHIAttachTextureInfoOpenGL &>(attach_color_attachment_info);
  glNamedFramebufferTexture(fbo, attachment, dynamic_cast<RHITextureOpenGL const *>(texture)->id, level);
  if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("frame buffer not complete");
  }
  return OpenGLCheckError();
}
// auto RHIOpenGL::AttachDepthAttachment();
// auto RHIOpenGL::AttachStencilAttachment();
// auto RHIOpenGL::AttachDepthStencilAttachment();

auto RHIOpenGL::Draw(
    RHIVertexArray const *vertex_array, RHIShaderProgram const *shader_program,
    std::optional<RHIFramebuffer const *> framebuffer
) -> bool {
  OpenGLCheckError();
  if (framebuffer.has_value()) {
    auto fbo = dynamic_cast<RHIFramebufferOpenGL const *>(framebuffer.value())->id;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    OpenGLCheckError();
  }
  glUseProgram(dynamic_cast<RHIShaderProgramOpenGL const *>(shader_program)->id);
  auto *vertex_array_opengl = dynamic_cast<RHIVertexArrayOpenGL const *>(vertex_array);
  glBindVertexArray(vertex_array_opengl->id);
  glDrawElements(GL_TRIANGLES, vertex_array_opengl->count, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return OpenGLCheckError();
}

}  // namespace nanoR