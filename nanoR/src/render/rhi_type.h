#pragma once

namespace nanoR {
struct RHIBuffer {
  virtual ~RHIBuffer() = default;
};

struct RHIBufferCreateInfo {
  virtual ~RHIBufferCreateInfo() = default;
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

}  // namespace nanoR