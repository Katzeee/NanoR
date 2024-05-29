#include "rhi_opengl4.h"

#include <glslc/src/file_includer.h>
#include <libshaderc_util/file_finder.h>

#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

#include "nanorpch.h"
#include "rhi_type_opengl.h"

namespace nanoR {

auto RHIOpenGL4::OpenGLCheckError() -> bool {
  if (auto res = glGetError(); res != GL_NO_ERROR) {
    LOG_FATAL("OpenGL API error {}\n", res);
    throw std::runtime_error("OpenGL API error");
  }
  return true;
}

auto RHIOpenGL4::CreateBuffer(const RHIBufferDesc &desc, const ResourceCreateInfo &info) -> std::shared_ptr<RHIBuffer> {
  auto buffer = std::make_shared<RHIBufferOpenGL>();
  OpenGLCheck(glCreateBuffers(1, &buffer->id));
  switch (desc.usage) {
  case EBufferUsage::STATIC:
    // TODO: STATIC
  case EBufferUsage::DYNAMIC:
  case EBufferUsage::UNIFORM:
  case EBufferUsage::INDEX:
  case EBufferUsage::VERTEX:
    OpenGLCheck(glNamedBufferStorage(buffer->id, desc.size, info.data, GL_DYNAMIC_STORAGE_BIT));
    break;
  default:
    LOG_ERROR("Not implemented");
    break;
  }
  buffer->desc = desc;
  return buffer;
}

auto RHIOpenGL4::UpdateBufferData(const RHIUpdateBufferDataInfo &info, const std::shared_ptr<RHIBuffer> &buffer) -> void {
  auto buffer_opengl = dynamic_cast<RHIBufferOpenGL *>(buffer.get());
  OpenGLCheck(glNamedBufferSubData(buffer_opengl->id, info.offset, info.size, info.data));
}

void RHIOpenGL4::DrawIndexed(const std::shared_ptr<RHIBuffer> &vertex_buffer, const std::shared_ptr<RHIBuffer> &index_buffer,
    RHIShaderProgram const *shader_program, std::optional<RHIFramebuffer const *> framebuffer) {
  unsigned int VAO;
  OpenGLCheck(glCreateVertexArrays(1, &VAO));
  // OpenGLCheck(glVertexArrayVertexBuffer(VAO, 0, dynamic_cast<RHIBufferOpenGL *>(vertex_buffer.get())->id, 0, ))
  // OpenGLCheck(glEnableVertexArrayAttrib(VAO, ))
}

auto RHIOpenGL4::BindUniformBuffer(const RHIBindUniformBufferInfoOpenGL &bind_uniform_buffer_info, RHIBuffer *buffer) -> bool {
  const auto &[target, index] = bind_uniform_buffer_info;
  auto buffer_opengl = dynamic_cast<RHIBufferOpenGL *>(buffer);
  glBindBufferBase(target, index, buffer_opengl->id);
  return OpenGLCheckError();
}

auto RHIOpenGL4::CreateVertexArray(std::shared_ptr<RHIVertexArray> &vertex_array) -> bool {
  auto *vertex_array_opengl = new RHIVertexArrayOpenGL{};
  glCreateVertexArrays(1, &vertex_array_opengl->id);
  vertex_array.reset(vertex_array_opengl);
  return OpenGLCheckError();
}

auto RHIOpenGL4::BindVertexBuffer(
    const RHIBindVertexBufferInfo &bind_vertex_buffer_info, std::shared_ptr<RHIVertexArray> vertex_array,
    std::shared_ptr<RHIBuffer> vertex_buffer) -> bool {
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
auto RHIOpenGL4::BindIndexBuffer(
    const RHIBindIndexBufferInfo &bind_index_buffer_info, std::shared_ptr<RHIVertexArray> vertex_array,
    std::shared_ptr<RHIBuffer> index_buffer) -> bool {
  auto *vertex_array_opengl = dynamic_cast<RHIVertexArrayOpenGL *>(vertex_array.get());
  auto *buffer_opengl = dynamic_cast<RHIBufferOpenGL *>(index_buffer.get());
  const auto &[count] = dynamic_cast<const RHIBindIndexBufferInfoOpenGL &>(bind_index_buffer_info);
  vertex_array_opengl->count = count;
  glVertexArrayElementBuffer(vertex_array_opengl->id, buffer_opengl->id);
  return OpenGLCheckError();
}

auto RHIOpenGL4::CreateShaderModule(
    const RHIShaderModuleCreateInfo &shader_module_create_info, std::shared_ptr<RHIShaderModule> &shader_module) -> bool {
  auto *shader_module_opengl = new RHIShaderModuleOpenGL{};
  // const auto &shader_module_create_info_opengl
  const auto &[type, file, src, count, length] =
      dynamic_cast<const RHIShaderModuleCreateInfoOpenGL &>(shader_module_create_info);
  // SECTION: compile to spirv
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;
  shaderc_util::FileFinder fileFinder;
  options.SetIncluder(std::make_unique<glslc::FileIncluder>(&fileFinder));
  options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
  const bool optimize = false; // for debug info
  if (optimize) {
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
  }
  // TODO: support other kind of shaders
  auto kind = type == GL_VERTEX_SHADER ? shaderc_shader_kind::shaderc_glsl_vertex_shader
                                       : shaderc_shader_kind::shaderc_glsl_fragment_shader;
  auto result = compiler.CompileGlslToSpv(src, kind, file, options);
  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    LOG_ERROR("{}\n", result.GetErrorMessage());
    return false;
  }
  auto &spirv_code = shader_module_opengl->spirv_code;
  spirv_code = {result.cbegin(), result.cend()};

  // SECTION:

  shader_module_opengl->id = glCreateShader(type);
  glShaderBinary(
      1, &shader_module_opengl->id, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv_code.data(),
      spirv_code.size() * sizeof(uint32_t));
  glSpecializeShader(shader_module_opengl->id, "main", 0, nullptr, nullptr);
  // glShaderSource(shader_module_opengl->id, count, &src, length);
  // glCompileShader(shader_module_opengl->id);
  int success;
  char info[512];
  glGetShaderiv(shader_module_opengl->id, GL_COMPILE_STATUS, &success);
  if (!static_cast<bool>(success)) {
    glGetShaderInfoLog(shader_module_opengl->id, 512, nullptr, info);
    LOG_ERROR("ERROR::SHADER::{}::COMPILATION_FAILED: {}\n", type, info);
  }
  shader_module.reset(shader_module_opengl);

  return OpenGLCheckError();
}

auto RHIOpenGL4::CreateShaderProgram(
    const RHIShaderProgramCreateInfo &shader_program_create_info, std::shared_ptr<RHIShaderProgram> &shader_program) -> bool {
  const auto &[shaders] = dynamic_cast<const RHIShaderProgramCreateInfoOpenGL &>(shader_program_create_info);
  auto *shader_program_opengl = new RHIShaderProgramOpenGL{};
  shader_program_opengl->id = glCreateProgram();
  for (auto &&shader : shaders) {
    glAttachShader(shader_program_opengl->id, dynamic_cast<RHIShaderModuleOpenGL *>(shader.get())->id);

    // SECTION: reflect
    const auto &spirv_code = shader->spirv_code;
    spirv_cross::Compiler spirv_compiler(spirv_code);
    spirv_cross::ShaderResources resources = spirv_compiler.get_shader_resources();
    for (const auto &resource : resources.uniform_buffers) {
      const auto &buffer_type = spirv_compiler.get_type(resource.base_type_id);
      unsigned set = spirv_compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
      unsigned binding = spirv_compiler.get_decoration(resource.id, spv::DecorationBinding);
      // ignore all ubo starts_with _
      if (resource.name.starts_with('_')) {
        continue;
      }

      // std::cout << "Uniform buffer '" << resource.name << "' at set = " << set << ", binding = " << binding << "\n";

      std::unordered_map<std::string, UniformBufferDesc::UniformVariable> vars;
      for (uint32_t i = 0; i < buffer_type.member_types.size(); ++i) {
        std::variant<int, float, glm::vec3, glm::vec4> value;
        const auto &member_type = spirv_compiler.get_type(buffer_type.member_types[i]);
        std::string member_name = spirv_compiler.get_member_name(buffer_type.self, i);

        size_t offset = spirv_compiler.type_struct_member_offset(buffer_type, i);
        // std::cout << "  Member name: " << member_name << ", Offset: " << offset << "\n";
        if (member_type.vecsize == 3) {
          // std::cout << "  Vector size: " << member_type.vecsize;
          value = glm::vec3{1};
        } else if (member_type.vecsize == 4) {
          // std::cout << "  Vector size: " << member_type.vecsize;
          value = glm::vec4{1};
        } else if (member_type.basetype == spirv_cross::SPIRType::Float) {
          LOG_TRACE("Member name: {}, {}\n", member_name, member_type.basetype == spirv_cross::SPIRType::Float);
          value = 1.0F;
        } else if (member_type.basetype == spirv_cross::SPIRType::Int) {
          value = 0;
        }

        // 如果成员是数组，打印数组的尺寸
        if (!member_type.array.empty()) {
          // std::cout << ", Array size: " << member_type.array[0];
          for (size_t j = 1; j < member_type.array.size(); ++j) {
            // std::cout << "[" << member_type.array[j] << "]";
          }
        }
        // vars.emplace_back(offset, member_name, value);
        vars[member_name] = {offset, value};
      }
      shader_program_opengl->ubo_descs[resource.name] = {binding, nullptr, std::move(vars)};
    }
  }

  // SECTION: create program
  glLinkProgram(shader_program_opengl->id);
  int success;
  char info[512];
  glGetProgramiv(shader_program_opengl->id, GL_LINK_STATUS, &success);
  if (!static_cast<bool>(success)) {
    glGetProgramInfoLog(shader_program_opengl->id, 512, nullptr, info);
    LOG_FATAL("ERROR::SHADER::PROGRAM::LINKING_FAILED: {}\n", info);
  }
  // SECTION: create ubo
  for (auto &ubo_desc : shader_program_opengl->ubo_descs) {
    auto size = 0;
    for (auto const &var : ubo_desc.second.vars) {
      std::visit([&](auto &&arg) { size += sizeof(decltype(arg)); }, var.second.value);
    }
    LOG_TRACE("size: {}\n", size);
    auto *buffer_opengl = new RHIBufferOpenGL{};
    glCreateBuffers(1, &buffer_opengl->id);
    glNamedBufferStorage(buffer_opengl->id, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, ubo_desc.second.binding, buffer_opengl->id);
    ubo_desc.second.ubo.reset(buffer_opengl);
  }

  shader_program.reset(shader_program_opengl);
  return OpenGLCheckError();
}

// auto RHIOpenGL::SetShaderUniform(
//     RHISetShaderUniformInfo const &set_shader_uniform_info, RHIShaderProgram *shader_program
// ) -> bool {
//   const auto &[uniforms] = set_shader_uniform_info;
//   for (auto &&uniform : uniforms) {
//     std::visit(
//         [&](auto &&value) {
//           using T = std::decay_t<decltype(value)>;
//           dynamic_cast<RHIShaderProgramOpenGL *>(shader_program)->SetValue<T>(uniform.name, static_cast<T>(value));
//         },
//         uniform.value
//     );
//   }
//   return OpenGLCheckError();
// }
auto RHIOpenGL4::CreateFramebuffer(
    const RHIFramebufferCreateInfo &framebuffer_create_info, std::shared_ptr<RHIFramebuffer> &framebuffer) -> bool {
  auto *framebuffer_opengl = new RHIFramebufferOpenGL{};
  glCreateFramebuffers(1, &framebuffer_opengl->id);
  framebuffer.reset(framebuffer_opengl);
  return OpenGLCheckError();
}

auto RHIOpenGL4::CreateTexture(const RHITextureCreateInfo &texture_create_info, std::shared_ptr<RHITexture> &texture)
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

auto RHIOpenGL4::AttachTexture(
    const RHIAttachTextureInfo &attach_color_attachment_info, RHIFramebuffer const *framebuffer,
    RHITexture const *texture) -> bool {
  const auto &[fbo] = *dynamic_cast<RHIFramebufferOpenGL const *>(framebuffer);
  const auto &[level, attachment] = dynamic_cast<const RHIAttachTextureInfoOpenGL &>(attach_color_attachment_info);
  if (texture) {
    glNamedFramebufferTexture(fbo, attachment, dynamic_cast<RHITextureOpenGL const *>(texture)->id, level);
  } else {
    glNamedFramebufferTexture(fbo, attachment, 0, level);
  }
  if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("frame buffer not complete");
  }
  return OpenGLCheckError();
}
// auto RHIOpenGL::AttachDepthAttachment();
// auto RHIOpenGL::AttachStencilAttachment();
// auto RHIOpenGL::AttachDepthStencilAttachment();

auto RHIOpenGL4::Draw(
    RHIVertexArray const *vertex_array, RHIShaderProgram const *shader_program,
    std::optional<RHIFramebuffer const *> framebuffer) -> bool {
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

} // namespace nanoR