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

namespace EventCategory {

// HINT: enum class cannot use operator |
enum type {
  kUnknown = 1,
  kInput = 1 << 1,
  kKey = 1 << 2,
  kMouse = 1 << 3,
  kWindow = 1 << 4,

  kMaxSize = 1 << 30,
};
}  // namespace EventCategory

#define DECLARE_EVENT_TYPE(event_type)         \
  static auto Type()->EventType {              \
    return EventType::event_type;              \
  }                                            \
  auto GetType() const->EventType override {   \
    return EventType::event_type;              \
  }                                            \
  auto GetName() const->std::string override { \
    return "Event::" #event_type;              \
  }

#define DECLARE_EVENT_CATEGORY(event_category)  \
  auto GetCategory() const->uint32_t override { \
    return event_category;                      \
  }

class Event {
 public:
  virtual auto ToString() const -> std::string {
    return fmt::format("[{}] ToString NOT IMPLEMENTED", GetName());
  }
  virtual auto GetType() const -> EventType {
    return EventType::kUnknown;
  }
  virtual auto GetName() const -> std::string {
    return "EventType::kUnknown";
  }
  virtual auto GetCategory() const -> uint32_t = 0;
};

}  // namespace nanoR