#pragma once
#include "event.h"

namespace nanoR {

struct MouseCursorMoveEvent final : public Event {
  DECLARE_EVENT_TYPE(kMouseCursorMove)
  DECLARE_EVENT_CATEGORY(EventCategory::kInput | EventCategory::kMouse)

  MouseCursorMoveEvent(double xpos, double ypos) : xpos(xpos), ypos(ypos) {}
  auto ToString() const -> std::string override {
    return fmt::format("[MouseCursorMoveEvent] xpos: {}, ypos: {}", xpos, ypos);
  }

  // For easy completion and less length, use xpos_ not pos_x_
  double xpos;
  double ypos;
};

struct MouseButtonDownEvent final : public Event {
  DECLARE_EVENT_TYPE(kMouseButtonDown)
  DECLARE_EVENT_CATEGORY(EventCategory::kInput | EventCategory::kMouse)

  MouseButtonDownEvent(int button_code, int mods) : button_code(button_code), mods(mods) {}
  auto ToString() const -> std::string override {
    return fmt::format("[MouseButtonDownEvent] button: {}, mods: {}", button_code, mods);
  }

  int button_code;
  int mods;
};

struct MouseButtonUpEvent final : public Event {
  DECLARE_EVENT_TYPE(kMouseButtonUp)
  DECLARE_EVENT_CATEGORY(EventCategory::kInput | EventCategory::kMouse)

  MouseButtonUpEvent(int button_code, int mods) : button_code(button_code), mods(mods) {}
  auto ToString() const -> std::string override {
    return fmt::format("[MouseButtonUpEvent] button: {}, mods: {}", button_code, mods);
  }

  int button_code;
  int mods;
};

class MouseButtonScrollEvent final : public Event {
 public:
  DECLARE_EVENT_TYPE(kMouseButtonScroll)
  DECLARE_EVENT_CATEGORY(EventCategory::kInput | EventCategory::kMouse)

  MouseButtonScrollEvent(double xoffset, double yoffset) : xoffset_(xoffset), yoffset_(yoffset) {}
  auto ToString() const -> std::string override {
    return fmt::format("[MouseButtonScrollEvent] xoffset: {}, yoffset: {}", xoffset_, yoffset_);
  }

 private:
  double xoffset_;
  double yoffset_;
};

}  // namespace nanoR