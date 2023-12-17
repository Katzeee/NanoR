#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "utils.hpp"

struct GLFWwindow;

namespace xac {
class ImguiLayer {
 public:
  ImguiLayer();
  auto Init(GLFWwindow *window) -> void;
  auto Watch(WatchVarBase *watch_var) -> void;
  auto Render() -> void;
  template <typename T>
  auto RegisterWatchVar(const std::string &name, T &&init_val, typename WatchVar<T>::WatchFunc &&f) -> void;
  template <typename T>
  auto GetWatchVar(const std::string &name) -> WatchVar<T> &;

 private:
  std::unordered_map<std::string, std::unique_ptr<WatchVarBase>> watch_vars_;
};

template <typename T>
auto ImguiLayer::RegisterWatchVar(const std::string &name, T &&init_val, typename WatchVar<T>::WatchFunc &&f) -> void {
  if (watch_vars_.contains(name)) {
    throw std::runtime_error("duplicate watch var name");
  }
  watch_vars_[name] = std::make_unique<WatchVar<T>>(std::forward<T>(init_val), std::move(f));
}

template <typename T>
auto ImguiLayer::GetWatchVar(const std::string &name) -> WatchVar<T> & {
  if (!watch_vars_.contains(name)) {
    throw std::runtime_error("No watch var named " + name);
  }
  return *dynamic_cast<WatchVar<T> *>(watch_vars_[name].get());
}
}  // namespace xac
