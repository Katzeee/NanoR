#pragma once
#include "event.h"

namespace nanoR {

struct KeyDownEvent final : public Event {
  DECLARE_EVENT_TYPE(kKeyDown)
  DECLARE_EVENT_CATEGORY(EventCategory::kInput | EventCategory::kKey)

  KeyDownEvent(int key_code, int mods) : key_code(key_code), mods(mods) {}
  auto ToString() const -> std::string override {
    return fmt::format("[KeyDownEvent] button: {}, mods: {}", key_code, mods);
  }

  int key_code;
  int mods;
};

struct KeyUpEvent final : public Event {
  DECLARE_EVENT_TYPE(kKeyUp)
  DECLARE_EVENT_CATEGORY(EventCategory::kInput | EventCategory::kKey)

  KeyUpEvent(int key_code, int mods) : key_code(key_code), mods(mods) {}
  auto ToString() const -> std::string override {
    return fmt::format("[KeyUpEvent] button: {}, mods: {}", key_code, mods);
  }

  int key_code;
  int mods;
};

}  // namespace nanoR