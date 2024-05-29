#pragma once
#include "rhi_type.h"

namespace nanoR {

struct RHIBuffer {
  virtual ~RHIBuffer() = default;
  RHIBufferDesc desc;
};

} // namespace nanoR