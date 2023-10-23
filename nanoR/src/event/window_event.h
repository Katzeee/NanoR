#pragma once
#include "event.h"

namespace nanoR {

class WindowResizeEvent final : public Event {
 public:
  DECLARE_EVENT_TYPE(kWindowResize)

  WindowResizeEvent(int width, int height) : width_(width), height_(height) {}
  auto ToString() -> std::string override {
    return fmt::format("[WindowResizeEvent] width: {}, height: {}\n", width_, height_);
  }

 private:
  int width_;
  int height_;
};

class WindowCloseEvent final : public Event {
 public:
  DECLARE_EVENT_TYPE(kWindowClose)

  WindowCloseEvent() = default;
  auto ToString() -> std::string override {
    return fmt::format("[WindowCloseEvent]\n");
  }
};

}  // namespace nanoR