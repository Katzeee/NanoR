#pragma once
#include <GLFW/glfw3.h>
#include "logger/logger.h"
#include "window.h"

namespace nanoR {
class LinuxWindow final : public Window {
 public:
  LinuxWindow() : LinuxWindow(WindowProp{}) {}
  LinuxWindow(WindowProp window_prop);
  auto Init(WindowProp window_prop = WindowProp{}) -> void override;
  auto Tick() -> void override;
  auto Shutdown() -> void override;
  ~LinuxWindow() override;

 private:
  GLFWwindow *window_;
};
}  // namespace nanoR