#pragma once
#include "nanorpch.h"

namespace nanoR {

enum class ControlCommand : uint32_t {
  kForward = 1 << 0,
  kBackward = 1 << 1,
  kLeft = 1 << 2,
  kRight = 1 << 3,
  kUp = 1 << 4,
  kDown = 1 << 5,
  kLeftButtonDown = 1 << 6,
  kRightButtonDown = 1 << 7,
};

enum class Platform {
  Winodws,
  Linux,
};

template <Platform T>
class InputSystem {};

}  // namespace nanoR