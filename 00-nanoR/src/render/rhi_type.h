#pragma once

namespace nanoR {

class RHIBuffer;

struct ResourceCreateInfo {
  void *data;
};

enum class EBufferUsage {
  STATIC,
  DYNAMIC,
  UNIFORM,
  VERTEX,
  INDEX,
};

struct RHIBufferDesc {
  uint32_t size;
  EBufferUsage usage;
};

struct RHIUpdateBufferDataInfo {
  uint32_t size;
  const void *data;
  uint16_t offset;
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

} // namespace nanoR

namespace nanoR {} // end namespace nanoR