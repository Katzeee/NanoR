#pragma once
#include <string_view>

namespace nanoR {

// clang-format off
enum class EventTypes {
  kUnkown,
  kWindowResize,
  kKeyDown, kKeyUp, 
  kButtonDown, kButtonUp
};
// clang-format on 

class Event {
public:
  virtual auto ToString() -> std::string = 0;


};
}  // namespace nanoR