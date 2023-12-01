#pragma once
#include <memory>

namespace xac {

enum class ControlCommand : uint32_t;
class Camera;
class InputSystem;
class Window;
class ImguiLayer;

class GlobalContext {
 public:
  GlobalContext() = default;
  ~GlobalContext() = default;

  void Init();

  std::unique_ptr<InputSystem> input_system_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<Window> window_;
  std::unique_ptr<ImguiLayer> imgui_layer_;

  uint32_t control_commad_ = 0;
  int window_width_ = 1540;
  int window_height_ = 900;
  int imgui_width_ = 340;

 private:
};

auto InCommand(xac::ControlCommand command) -> bool;

}  // end namespace xac

extern xac::GlobalContext global_context;