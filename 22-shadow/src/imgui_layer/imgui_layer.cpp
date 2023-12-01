#include "imgui_layer.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

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
}

}  // namespace xac