#pragma once
#include <GLFW/glfw3.h>

#include "window/window_base.h"
#include "window/window_prop.h"

namespace nanoR {

class WindowLinux final : public WindowBase {
 public:
  WindowLinux() : WindowLinux(WindowProp{}) {}
  WindowLinux(WindowProp window_prop);
  auto Init(WindowProp window_prop = WindowProp{}) -> void override;
  auto Tick() -> void override;
  auto Shutdown() -> void override;
  auto GetRawWindow() -> void * override;
  ~WindowLinux() override;

  auto WindowResizeCallback(GLFWwindow *window, int width, int height) -> void;

 private:
  GLFWwindow *window_ = nullptr;
};
}  // namespace nanoR