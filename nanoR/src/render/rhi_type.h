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
  virtual ~RHIShaderModule() = default;
};

struct RHIShaderProgramCreateInfo {
  virtual ~RHIShaderProgramCreateInfo() = default;
};

struct RHIShaderProgram {
  virtual ~RHIShaderProgram() = default;
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