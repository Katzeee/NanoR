#pragma once
#include <memory>

#include "nanorpch.h"

namespace nanoR {
class Application {
 public:
  Application();
  virtual ~Application() = default;
  virtual auto Init() -> void;
  virtual auto Tick() -> void;
  auto EventCallback(std::shared_ptr<Event> event) -> void;

 protected:
  std::unique_ptr<Window> window_;
  bool is_running_ = false;
};
}  // namespace nanoR