#include "ui_layer.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "global/global_context.h"
#include "nanorpch.h"
#include "platform/input_system_glfw.h"
#include "platform/opengl/rhi_opengl.h"
#include "platform/window_linux.h"
#include "render/camera.h"
#include "scene/component.hpp"
#include "scene/scene.hpp"

namespace nanoR {

auto UILayer::OnAttach() -> void {
  rhi_ = GlobalContext::Instance().rhi;
  CreateSceneFramebuffer();

  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  auto window = static_cast<GLFWwindow*>(GlobalContext::Instance().window->GetRawWindow());
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 450 core");
  ImGui::StyleColorsDark();
}

auto UILayer::CreateSceneFramebuffer() -> void {
  rhi_->CreateFramebuffer({}, scene_framebuffer_);
  scene_color_attachment_create_info_.internal_format = GL_RGBA8;
  scene_color_attachment_create_info_.levels = 1;
  scene_color_attachment_create_info_.target = GL_TEXTURE_2D;
  scene_color_attachment_create_info_.data = nullptr;
  scene_color_attachment_create_info_.parameteri.push_back({GL_TEXTURE_MIN_FILTER, GL_LINEAR});
  scene_color_attachment_create_info_.parameteri.push_back({GL_TEXTURE_MAG_FILTER, GL_LINEAR});
  scene_color_attachment_create_info_.parameteri.push_back({GL_TEXTURE_WRAP_S, GL_REPEAT});
  scene_color_attachment_create_info_.parameteri.push_back({GL_TEXTURE_WRAP_T, GL_REPEAT});
  scene_color_attachment_create_info_.width = GlobalContext::Instance().window->window_prop_.width;
  scene_color_attachment_create_info_.height = GlobalContext::Instance().window->window_prop_.height;
  rhi_->CreateTexture(scene_color_attachment_create_info_, scene_color_attachment_);
  scene_depth_attachment_create_info_ = scene_color_attachment_create_info_;
  scene_depth_attachment_create_info_.internal_format = GL_DEPTH_COMPONENT24;
  rhi_->CreateTexture(scene_depth_attachment_create_info_, scene_depth_attachment_);
  auto attach_info = nanoR::RHIAttachTextureInfoOpenGL{};
  attach_info.level = 0;
  attach_info.attachment = GL_COLOR_ATTACHMENT0;
  rhi_->AttachTexture(attach_info, scene_framebuffer_.get(), scene_color_attachment_.get());
  attach_info.attachment = GL_DEPTH_ATTACHMENT;
  rhi_->AttachTexture(attach_info, scene_framebuffer_.get(), scene_depth_attachment_.get());
}

auto UILayer::GetSceneFramebuffer() -> RHIFramebuffer* {
  return scene_framebuffer_.get();
}

auto UILayer::OnDetach() -> void {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

auto UILayer::Tick(uint64_t delta_time) -> void {
  Begin();
  Scene();
  Input();
  Hierarchy();
  Inspector();
  ImGui::ShowDemoWindow();
  End();
}

auto UILayer::OnEvent(std::shared_ptr<Event> const& event) -> bool {
  return passthough_event_;
}

auto UILayer::Begin() -> void {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  DockSpace();
}

auto UILayer::DockSpace() -> void {
  static bool dockspaceOpen = true;
  static bool opt_fullscreen_persistant = true;
  bool opt_fullscreen = opt_fullscreen_persistant;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  if (opt_fullscreen) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |=
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  }

  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
  ImGui::PopStyleVar();

  if (opt_fullscreen) ImGui::PopStyleVar(2);

  // DockSpace
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigWindowsMoveFromTitleBarOnly = true;
  ImGuiStyle& style = ImGui::GetStyle();
  float minWinSizeX = style.WindowMinSize.x;
  style.WindowMinSize.x = 370.0f;
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }
  ImGui::End();
}

auto UILayer::Scene() -> void {
  ImGui::Begin("Scene");
  SetBlockEvent(ImGui::IsItemHovered() || !(ImGui::IsWindowFocused() && ImGui::IsWindowHovered()));

  ImVec2 scene_size = ImGui::GetContentRegionAvail();
  if (scene_size.x != scene_color_attachment_create_info_.width ||
      scene_size.y != scene_color_attachment_create_info_.height) {
    scene_color_attachment_create_info_.width = scene_size.x;
    scene_color_attachment_create_info_.height = scene_size.y;
    scene_depth_attachment_create_info_.width = scene_size.x;
    scene_depth_attachment_create_info_.height = scene_size.y;
    rhi_->CreateTexture(scene_color_attachment_create_info_, scene_color_attachment_);
    rhi_->CreateTexture(scene_depth_attachment_create_info_, scene_depth_attachment_);
    auto attach_info = nanoR::RHIAttachTextureInfoOpenGL{};
    attach_info.level = 0;
    attach_info.attachment = GL_COLOR_ATTACHMENT0;
    rhi_->AttachTexture(attach_info, scene_framebuffer_.get(), scene_color_attachment_.get());
    attach_info.attachment = GL_DEPTH_ATTACHMENT;
    rhi_->AttachTexture(attach_info, scene_framebuffer_.get(), scene_depth_attachment_.get());
    LOG_TRACE("scene size: {} {}\n", scene_size.x, scene_size.y);
    glViewport(0, 0, scene_size.x, scene_size.y);
    GlobalContext::Instance().main_camera->SetAspect(scene_size.x / scene_size.y);
  }
  ImGui::Image(
      reinterpret_cast<void*>((dynamic_cast<nanoR::RHITextureOpenGL*>(scene_color_attachment_.get())->id)), scene_size,
      {0, 1}, {1, 0}
  );
  ImGui::End();
}

auto UILayer::Input() -> void {
  ImGui::Begin("Input");
  ImGui::Text("Commands: %d", nanoR::GlobalContext::Instance().input_system->control_commad);
  ImGui::Text("Scroll: %lf", nanoR::GlobalContext::Instance().input_system->scroll_y_offset);
  // auto cam_pos = main_camera_.GetPosition();
  // ImGui::Text("Position: %f, %f, %f", cam_pos.x, cam_pos.y, cam_pos.z);
  // ImGui::Text(
  //     "x_offset: %.10f, y_offset: %.10f", nanoR::GlobalContext::Instance().input_system->cursor_x_offset,
  //     nanoR::GlobalContext::Instance().input_system->cursor_x_offset
  // );
  // ImGui::Text("yaw: %f, pitch: %f", main_camera_.yaw_, main_camera_.pitch_);
  ImGui::End();
}

auto UILayer::Hierarchy() -> void {
  static ImGuiTreeNodeFlags base_flags =
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
  ImGui::Begin("Hierarchy");
  if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
    static int selected = -1;
    GlobalContext::Instance().scene->Each([&](Entity& entity, uint32_t i) {
      auto c_name = entity.GetComponenet<NameComponent>();
      auto node_flags = base_flags | ImGuiTreeNodeFlags_Leaf;
      if (selected == i) {
        node_flags |= ImGuiTreeNodeFlags_Selected;
      }
      bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "%s", c_name->name.c_str());

      if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        selected = i;
        selected_entity_ = entity;
      };
      if (node_open) {
        ImGui::TreePop();
      }
    });
    ImGui::TreePop();
  }
  ImGui::End();
}

auto UILayer::Inspector() -> void {
  ImGui::Begin("inspector");
  if (!selected_entity_) {
    ImGui::End();
    return;
  }

  {
    auto c_name = selected_entity_.GetComponenet<NameComponent>();
    ImGui::Text("%s", c_name->name.c_str());
  }
  {
    if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
      // ImGui::PushID()
      auto c_transform = selected_entity_.GetComponenet<TransformComponent>();
      ImGui::DragFloat3("Position", glm::value_ptr(c_transform->position), 0.01F);

      auto eular = glm::degrees(c_transform->rotation);
      ImGui::DragFloat3("Rotation", glm::value_ptr(eular), 1.0F);
      eular = glm::radians(eular);
      if (auto res = glm::epsilonNotEqual(eular, c_transform->rotation, glm::epsilon<float>()); glm::any(res)) {
        c_transform->rotation = eular;
      }

      ImGui::DragFloat3("Scale", glm::value_ptr(c_transform->scale), 0.01F);
      ImGui::TreePop();
    }
  }
  {
    auto c_light = selected_entity_.GetComponenet<LightCompoenent>();
    if (c_light.get()) {
      if (ImGui::TreeNodeEx("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit3("Color", glm::value_ptr(c_light->light->GetColor()));
        ImGui::DragFloat("Intensity", &c_light->light->GetIntensity());
        ImGui::TreePop();
      }
    }
  }
  {
    auto c_mesh_renderer = selected_entity_.GetComponenet<MeshRendererCompoenent>();
    if (c_mesh_renderer.get()) {
      auto& c_materials = c_mesh_renderer->materials;
      if (ImGui::TreeNodeEx("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
          for (auto&& it : c_materials) {
            ImGui::Text("%s", it->GetName().data());
            ImGui::ColorEdit4("base_color", glm::value_ptr(it->GetVec4("base_color")));
            // TODO: opengl
            ImGui::Text("albedo");
            ImGui::Image(
                reinterpret_cast<void*>(dynamic_cast<RHITextureOpenGL*>(it->GetTexture("albedo"))->id), ImVec2{64, 64}
            );
          }
          ImGui::TreePop();
        }
        ImGui::TreePop();
      }
    }
  }
  ImGui::End();
}

auto UILayer::End() -> void {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace nanoR