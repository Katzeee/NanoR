#include "global_context.h"

namespace nanoR {

auto GlobalContext::Instance() -> GlobalContext& {
  static GlobalContext instance_;
  return instance_;
}
}  // namespace nanoR