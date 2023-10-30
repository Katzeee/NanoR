#pragma once

#include "event/event.h"
#include "window_prop.h"

namespace nanoR {

class WindowBase {
 public:
  struct UserData {
    std::function<void(std::shared_ptr<Event>)> event_callback;
  };
  WindowBase() = default;
  virtual auto Init(WindowProp window_prop) -> void = 0;
  virtual auto Tick() -> void = 0;
  virtual auto Shutdown() -> void = 0;
  virtual auto GetRawWindow() -> void* = 0;
  virtual ~WindowBase() = default;

  UserData user_data_;
};
}  // namespace nanoR
