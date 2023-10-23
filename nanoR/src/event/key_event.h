#pragma once
#include "event.h"

namespace nanoR {

class KeyDownEvent final : public Event {
 public:
  DECLARE_EVENT_TYPE(kKeyDown)
  DECLARE_EVENT_CATEGORY(EventCategory::kInput | EventCategory::kKey)

  KeyDownEvent(int key_code, int mods) : key_code_(key_code), mods_(mods) {}
  auto ToString() const -> std::string override {
    return fmt::format("[KeyDownEvent] button: {}, mods: {}\n", key_code_, mods_);
  }

 private:
  int key_code_;
  int mods_;
};

class KeyUpEvent final : public Event {
 public:
  DECLARE_EVENT_TYPE(kKeyDown)
  DECLARE_EVENT_CATEGORY(EventCategory::kInput | EventCategory::kKey)

  KeyUpEvent(int key_code, int mods) : key_code_(key_code), mods_(mods) {}
  auto ToString() const -> std::string override {
    return fmt::format("[KeyUpEvent] button: {}, mods: {}\n", key_code_, mods_);
  }

 private:
  int key_code_;
  int mods_;
};

}  // namespace nanoR