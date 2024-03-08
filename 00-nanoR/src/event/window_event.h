#pragma once
#include "event.h"

namespace nanoR {

class WindowResizeEvent final : public Event {
 public:
  DECLARE_EVENT_TYPE(kWindowResize)
  DECLARE_EVENT_CATEGORY(EventCategory::kWindow)

  WindowResizeEvent(int width, int height) : width_(width), height_(height) {}
  auto ToString() const -> std::string override {
    return fmt::format("[WindowResizeEvent] width: {}, height: {}", width_, height_);
  }

 private:
  int width_;
  int height_;
};

class WindowCloseEvent final : public Event {
 public:
  DECLARE_EVENT_TYPE(kWindowClose)
  DECLARE_EVENT_CATEGORY(EventCategory::kWindow)

  WindowCloseEvent() = default;
  auto ToString() const -> std::string override {
    return fmt::format("[WindowCloseEvent]");
  }
};

}  // namespace nanoR