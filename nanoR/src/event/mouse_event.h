#pragma once
#include "event.h"
#include "fmt/core.h"

namespace nanoR {

class MouseCursorMoveEvent final : public Event {
 public:
  DECLARE_EVENT_TYPE(kMouseCursorMove)

  MouseCursorMoveEvent(double xpos, double ypos) : xpos_(xpos), ypos_(ypos) {}
  auto ToString() -> std::string override {
    return fmt::format("[MouseCursorMoveEvent] xpos: {}, ypos: {}\n", xpos_, ypos_);
  }

 private:
  // For easy completion and less length, use xpos_ not pos_x_
  double xpos_;
  double ypos_;
};

class MouseButtonDownEvent final : public Event {
 public:
  DECLARE_EVENT_TYPE(kMouseButtonDown)

  MouseButtonDownEvent(int button_code, int mods) : button_code_(button_code), mods_(mods) {}
  auto ToString() -> std::string override {
    return fmt::format("[MouseButtonDownEvent] button: {}, mods: {}\n", button_code_, mods_);
  }

 private:
  int button_code_;
  int mods_;
};

class MouseButtonUpEvent final : public Event {
 public:
  DECLARE_EVENT_TYPE(kMouseButtonUp)

  MouseButtonUpEvent(int button_code, int mods) : button_code_(button_code), mods_(mods) {}
  auto ToString() -> std::string override {
    return fmt::format("[MouseButtonUpEvent] button: {}, mods: {}\n", button_code_, mods_);
  }

 private:
  int button_code_;
  int mods_;
};

class MouseButtonScrollEvent final : public Event {
 public:
  DECLARE_EVENT_TYPE(kMouseButtonScroll)

  MouseButtonScrollEvent(double xoffset, double yoffset) : xoffset_(xoffset), yoffset_(yoffset) {}
  auto ToString() -> std::string override {
    return fmt::format("[MouseButtonScrollEvent] xoffset: {}, yoffset: {}\n", xoffset_, yoffset_);
  }

 private:
  double xoffset_;
  double yoffset_;
};

}  // namespace nanoR