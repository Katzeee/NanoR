#include "imgui_layer.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <algorithm>
#include <iostream>

namespace xac {
ImguiLayer::ImguiLayer() {
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
}

auto ImguiLayer::Init(GLFWwindow *window) -> void {
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 450 core");
  ImGui::StyleColorsDark();

  // int x;
  // WatchVar watch_x(&x, [](auto &&new_val) { std::cout << new_val << std::endl; });
}

// template <typename T, typename F>
// auto ImguiLayer::RegisterWatchVar(const std::string &name, T &&init_val, F &&f) -> void {
//   if (watch_vars_.contains(name)) {
//     throw std::runtime_error("duplicate watch var name");
//   }
//   watch_vars_[name] = std::make_unique<WatchVar<T, F>>(std::forward(init_val), std::forward(f));
// }

// auto ImguiLayer::Watch(WatchVarBase *watch_var) -> void {
//   watch_vars_.emplace_back(watch_var);
// }

auto ImguiLayer::Render() -> void {
  std::ranges::for_each(watch_vars_, [](auto &&var) { (*var.second)(); });
}

}  // namespace xac