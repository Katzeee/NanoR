#pragma once
#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils.hpp"

namespace xac {
class ImguiLayer {
 public:
  ImguiLayer();
  auto Init(GLFWwindow *window) -> void;
  auto Watch(WatchVarBase *watch_var) -> void;
  auto Render() -> void;
  template <typename T, typename F>
  auto RegisterWatchVar(const std::string &name, T &&init_val, F &&f) -> void;
  template<typename T>
  auto GetWatchVar(const std::string &name);

 private:
  std::unordered_map<std::string, std::unique_ptr<WatchVarBase>> watch_vars_;
};
}  // namespace xac
