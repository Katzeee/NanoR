#include "ui_layer_glfw_opengl3.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "global/global_context.h"
#include "nanorpch.h"
namespace nanoR {

auto UILayerGLFWOpenGL3::OnAttach() -> void {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  auto window = static_cast<GLFWwindow*>(GlobalContext::Instance().window_->GetRawWindow());
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 450 core");
  ImGui::StyleColorsDark();
}

auto UILayerGLFWOpenGL3::OnDetach() -> void {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

auto UILayerGLFWOpenGL3::OnEvent(Event& event) -> void {}

auto UILayerGLFWOpenGL3::Begin() -> void {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

auto UILayerGLFWOpenGL3::End() -> void {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace nanoR