#pragma once
#include <memory>

namespace xac {

enum class ControlCommand : uint32_t;
class Camera;
class InputSystem;

class GlobalContext {
 public:
  GlobalContext() = default;
  ~GlobalContext() = default;

  void Init();

  std::unique_ptr<InputSystem> input_system_;
  std::unique_ptr<Camera> camera_;

  static uint32_t control_commad_;

 private:
};

static auto InCommand(ControlCommand command) -> bool {
  return static_cast<uint32_t>(command) & GlobalContext::control_commad_;
}

}  // end namespace xac

extern xac::GlobalContext global_context;
extern int global_width;
extern int global_height;
extern const int global_imgui_width;