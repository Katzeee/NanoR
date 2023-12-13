#pragma once
#include "nanorpch.h"

namespace nanoR {

enum class ControlCommand : uint32_t {
  FORWARD = 1 << 0,
  BACKWARD = 1 << 1,
  LEFT = 1 << 2,
  RIGHT = 1 << 3,
  UP = 1 << 4,
  DOWN = 1 << 5,
  SHOW_CURSOR = 1 << 6,
};

enum class Platform {
  Winodws,
  Linux,
};

template <Platform T>
class InputSystem {
 private:
};


}  // namespace nanoR