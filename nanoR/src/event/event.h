#pragma once
#include "nanorpch.h"

namespace nanoR {

// clang-format off
enum class EventType {
  kUnknown,
  kWindowResize, kWindowClose, kWindowMove, kWindowFocus, kWindowUnFocus,
  kKeyDown, kKeyUp, 
  kMouseCursorMove, kMouseButtonDown, kMouseButtonUp, kMouseButtonScroll,
};
// clang-format on 


// HINT: enum class cannot use operator | 
struct EventCategory {
  static const uint32_t kUnknown  = 1;
  static const uint32_t kInput    = 1 << 1;
  static const uint32_t kKey      = 1 << 2;
  static const uint32_t kMouse    = 1 << 3;
  static const uint32_t kWindow   = 1 << 4;

  static const uint32_t kMaxSize  = 1 << 30;
};

#define DECLARE_EVENT_TYPE(event_type) static auto Type() -> EventType { return EventType::event_type; } \
                                       auto GetType() const -> EventType override { return EventType::event_type; } \
                                       auto GetName() const -> std::string override { return "Event::" #event_type; }

#define DECLARE_EVENT_CATEGORY(event_category) auto GetCategory() const -> uint32_t override { return event_category; }

class Event {
public:
  virtual auto ToString() const -> std::string { return fmt::format("[{}] ToString NOT IMPLEMENTED\n", GetName()); } 
  virtual auto GetType() const -> EventType { return EventType::kUnknown; } 
  virtual auto GetName() const -> std::string { return "EventType::kUnknown"; } 
  virtual auto GetCategory() const -> uint32_t = 0;
};

}  // namespace nanoR