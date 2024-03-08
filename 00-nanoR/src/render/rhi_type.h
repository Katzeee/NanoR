#pragma once

namespace nanoR {
struct RHIBuffer {
  virtual ~RHIBuffer() = default;
};

struct RHIBufferCreateInfo {
  virtual ~RHIBufferCreateInfo() = default;
};
struct RHISetBufferDataInfo {
  virtual ~RHISetBufferDataInfo() = default;
};

struct RHIVertexArray {
  virtual ~RHIVertexArray() = default;
};

struct RHIBindVertexBufferInfo {
  virtual ~RHIBindVertexBufferInfo() = default;
};

struct RHIBindIndexBufferInfo {
  virtual ~RHIBindIndexBufferInfo() = default;
};

struct RHIShaderModuleCreateInfo {
  virtual ~RHIShaderModuleCreateInfo() = default;
};

struct RHIShaderModule {
  std::vector<uint32_t> spirv_code;
  virtual ~RHIShaderModule() = default;
};

struct RHIShaderProgramCreateInfo {
  virtual ~RHIShaderProgramCreateInfo() = default;
};

struct UniformBufferDesc {
  struct UniformVariable {
    size_t offset;
    // std::string name;
    std::variant<int, float, glm::vec3, glm::vec4> value;
  };
  uint32_t binding;
  std::shared_ptr<RHIBuffer> ubo;
  std::unordered_map<std::string, UniformVariable> vars;
};

struct RHIShaderProgram {
  virtual ~RHIShaderProgram() = default;
  std::map<std::string, UniformBufferDesc> ubo_descs;
};

struct RHIBindUniformBufferInfo {
  virtual ~RHIBindUniformBufferInfo() = default;
};

struct RHIFramebufferCreateInfo {
  virtual ~RHIFramebufferCreateInfo() = default;
};

struct RHITextureCreateInfo {
  virtual ~RHITextureCreateInfo() = default;
};

struct RHITexture {
  virtual ~RHITexture() = default;
};

struct RHIFramebuffer {
  virtual ~RHIFramebuffer() = default;
};

struct RHIAttachTextureInfo {
  virtual ~RHIAttachTextureInfo() = default;
};

struct RHIAttachDepthAttachmentInfo {
  virtual ~RHIAttachDepthAttachmentInfo() = default;
};

struct RHIAttachStencilAttachmentInfo {
  virtual ~RHIAttachStencilAttachmentInfo() = default;
};

struct RHIAttachDepthStencilAttachmentInfo {
  virtual ~RHIAttachDepthStencilAttachmentInfo() = default;
};

}  // namespace nanoR