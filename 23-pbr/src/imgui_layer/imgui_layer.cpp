#include "imgui_layer.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <algorithm>
#include <iostream>

#include "context/context.hpp"

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

auto ImguiLayer::Render() -> void {
  std::ranges::for_each(watch_vars_, [](auto &&var) { (*var.second)(); });

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::SetNextWindowPos({0, 0});
  ImGui::SetNextWindowSize(
      {static_cast<float>(global_context.imgui_width_), static_cast<float>(global_context.window_height_)}
  );
  ImGui::Begin("Hello, World!", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
  ImGui::SetWindowFontScale(1.2);

  // ImGui::ColorEdit4("BG_color", reinterpret_cast<float *>(&background_color), ImGuiColorEditFlags_AlphaPreview);

  // if (ImGui::TreeNodeEx("Rotating light", ImGuiTreeNodeFlags_DefaultOpen)) {
  //   ImGui::ColorEdit3("p_light1", reinterpret_cast<float *>(&p_lights[0].color));
  //   ImGui::Separator();
  //   ImGui::ColorEdit3("p_light2", reinterpret_cast<float *>(&p_lights[1].color));
  //   ImGui::DragFloat3("rotation_axis", reinterpret_cast<float *>(&rotation_axis), 0.005f, -1.0f, 1.0f);
  //   ImGui::Separator();
  //   ImGui::ColorEdit3("d_light1", reinterpret_cast<float *>(&d_lights[0].color));
  //   ImGui::PushItemWidth(80);
  //   ImGui::SliderFloat("X", &d_lights[0].direction.x, -1.0f, 1.0f);
  //   ImGui::SameLine();
  //   ImGui::SliderFloat("Y", &d_lights[0].direction.y, -1.0f, 1.0f, "%.3f", ImGuiSliderFlags_None);
  //   ImGui::SameLine();
  //   ImGui::SliderFloat("Z", &d_lights[0].direction.z, -1.0f, 1.0f);
  //   ImGui::PopItemWidth();
  //   ImGui::TreePop();
  // }
  // ImGui::SliderFloat("Camera speed", &global_context.camera_->speed_, 5.0f, 30.0f);
  // ImGui::SliderFloat("rotation_degree", &rotation_degree, 0, 360);

  if (ImGui::TreeNodeEx("Depth test", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Combo("Debug Mode", GetWatchVar<int>("shader_debug_mode").Data(), "NODEBUG\0NORMAL\0DEPTH\0");
    ImGui::Combo(
        "Depth test", GetWatchVar<int>("gl_depth_func").Data(),
        "GL_NEVER\0GL_LESS\0GL_EQUAL\0GL_LEQUAL\0GL_GREATER\0GL_NOTEQUAL\0GL_GEQUAL\0GL_ALWAYS\0"
    );
    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("Face culling", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Checkbox("Face culling enable", GetWatchVar<bool>("cull_face_enable").Data());
    ImGui::SameLine();
    ImGui::Combo(
        "Face culling", GetWatchVar<int>("cull_face_mode").Data(),
        "GL_FRONT_LEFT\0GL_FRONT_RIGHT\0GL_BACK_LEFT\0GL_BACK_RIGHT\0GL_FRONT\0GL_BACK\0GL_LEFT\0GL_"
        "RIGHT\0GL_FRONT_AND_BACK\0"
    );
    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Combo("Lighting Model", GetWatchVar<int>("lighting_mode").Data(), "Phong\0Blinn Phong\0");
    ImGui::Combo("Shadow Model", GetWatchVar<int>("shadow_mode").Data(), "None\0PCF\0PCSS\0");
    ImGui::TreePop();
  }

  // if (ImGui::CollapsingHeader("Section 2", ImGuiTreeNodeFlags_DefaultOpen)) {
  //   if (ImGui::TreeNodeEx("Cursor", ImGuiTreeNodeFlags_DefaultOpen)) {
  //     ImGui::Text("yaw: %f\npitch: %f", global_context.camera_->GetYaw(), global_context.camera_->GetPitch());
  //     ImGui::TreePop();
  //   }

  //   if (ImGui::TreeNodeEx("Cursor", ImGuiTreeNodeFlags_DefaultOpen)) {
  //     ImGui::Text("xoffset: %f\nyoffset: %f", xac::InputSystem::cursor_x_offset_,
  //     xac::InputSystem::cursor_y_offset_); ImVec2 mouse_position_absolute = ImGui::GetMousePos();
  //     ImGui::Text("Position: %f, %f", mouse_position_absolute.x, mouse_position_absolute.y);
  //     ImGui::TreePop();
  //   }
  //   if (ImGui::TreeNodeEx("Command", ImGuiTreeNodeFlags_DefaultOpen)) {
  //     ImGui::Text(
  //         "FORWARD: %d, BACKWARD: %d", xac::InCommand(xac::ControlCommand::FORWARD),
  //         xac::InCommand(xac::ControlCommand::BACKWARD)
  //     );
  //     ImGui::Text(
  //         "LEFT: %4d, RIGHT: %4d", xac::InCommand(xac::ControlCommand::LEFT),
  //         xac::InCommand(xac::ControlCommand::RIGHT)
  //     );
  //     ImGui::Text(
  //         "DOWN: %4d, UP: %7d", xac::InCommand(xac::ControlCommand::DOWN), xac::InCommand(xac::ControlCommand::UP)
  //     );
  //     ImGui::TreePop();
  //   }
  //   ImGui::Text("Frame rate: %d", frame_count);
  // }
  ImGui::End();

  ImGui::Begin("Debug image");
  // FIX: Can't see anything without doing LinearizeDepth
  ImGui::Image(reinterpret_cast<void *>(13), ImVec2{256, 256});
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace xac