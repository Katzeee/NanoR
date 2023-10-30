#pragma once

#include "event/event.h"

namespace nanoR {
struct WindowProp {
  unsigned int height;
  unsigned int width;
  const char *title;

  WindowProp() : WindowProp(900, 1600, "Test") {}
  WindowProp(unsigned int height, unsigned int width, const char *title) : height(height), width(width), title(title) {}
};

class Window {
 public:
  struct UserData {
    std::function<void(std::shared_ptr<Event>)> event_callback;
  };
  Window() = default;
  virtual auto Init(WindowProp window_prop) -> void = 0;
  virtual auto Tick() -> void = 0;
  virtual auto Shutdown() -> void = 0;
  virtual auto GetRawWindow() -> void * = 0;
  virtual ~Window() = default;

  UserData user_data_;

 protected:
};
}  // namespace nanoR