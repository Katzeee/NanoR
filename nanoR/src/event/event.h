#pragma once
#include <string>

#include "fmt/core.h"

namespace nanoR {

// clang-format off
enum class EventType {
  kUnknown,
  kWindowResize, kWindowClose, kWindowMove, kWindowFocus, kWindowUnFocus,
  kKeyDown, kKeyUp, 
  kMouseButtonDown, kMouseButtonUp, kMouseButtonScroll, kMouseCursorMove,
};
// clang-format on 

#define DECLARE_EVENT_TYPE(event_name) static auto Type() -> EventType { return EventType::event_name; } \
                                       auto GetType() -> EventType override { return EventType::event_name; } \
                                       auto GetName() -> std::string override { return "Event::" #event_name; }

class Event {
public:
  virtual auto ToString() -> std::string { return fmt::format("[{}] ToString NOT IMPLEMENTED\n", GetName()); } 
  virtual auto GetType() -> EventType { return EventType::kUnknown; } 
  virtual auto GetName() -> std::string { return "EventType::kUnknown"; } 


};
}  // namespace nanoR