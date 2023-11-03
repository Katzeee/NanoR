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
}  // namespace nanoR