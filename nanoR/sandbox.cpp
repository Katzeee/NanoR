#include <memory>

#include "event/key_event.h"
#include "glad/glad.h"
#include "imgui.h"
#include "nanor.h"

class EditorLayer : public nanoR::Layer {
 public:
  EditorLayer(std::string name) : Layer(name) {
    scene_ = std::make_shared<nanoR::Scene>();
  }

  auto OnAttach() -> void override {
    auto e = scene_->CreateEntity();
    mesh_ = nanoR::CreateMesh(&rhi_, nanoR::ResourceManager::GetCubeMeshData());
    rhi_.CreateFramebuffer({}, fbo_);
    fbo_color_attachment_create_info_.internal_format = GL_RGBA8;
    fbo_color_attachment_create_info_.format = GL_RGB;
    fbo_color_attachment_create_info_.levels = 1;
    fbo_color_attachment_create_info_.target = GL_TEXTURE_2D;
    fbo_color_attachment_create_info_.type = GL_UNSIGNED_BYTE;
    fbo_color_attachment_create_info_.data = nullptr;
    fbo_color_attachment_create_info_.parameteri.push_back({GL_TEXTURE_MIN_FILTER, GL_LINEAR});
    fbo_color_attachment_create_info_.parameteri.push_back({GL_TEXTURE_MAG_FILTER, GL_LINEAR});
    fbo_color_attachment_create_info_.parameteri.push_back({GL_TEXTURE_WRAP_S, GL_REPEAT});
    fbo_color_attachment_create_info_.parameteri.push_back({GL_TEXTURE_WRAP_T, GL_REPEAT});
    fbo_color_attachment_create_info_.width = 1600;
    fbo_color_attachment_create_info_.height = 900;
    rhi_.CreateTexture(fbo_color_attachment_create_info_, t_fbo_color_attachment_);
    auto attach_color_attachment_info = nanoR::RHIAttachColorAttachmentInfoOpenGL{};
    attach_color_attachment_info.level = 0;
    rhi_.AttachColorAttachment(attach_color_attachment_info, fbo_.get(), t_fbo_color_attachment_.get());
    shader_program_ = nanoR::ResourceManager::GetUnlitShader(&rhi_);
  }

  auto Tick(uint64_t delta_time) -> void override {
    main_camera_.Tick(delta_time);
    glBindFramebuffer(GL_FRAMEBUFFER, dynamic_cast<nanoR::RHIFramebufferOpenGL*>(fbo_.get())->id);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto view = main_camera_.GetViewMatrix();
    auto proj = main_camera_.GetProjectionMatrix();
    dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(shader_program_.get())->SetValue("View", view);
    dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(shader_program_.get())->SetValue("Proj", proj);
    rhi_.Draw(mesh_.vao.get(), shader_program_.get(), fbo_.get());
  }

  auto TickUI() -> void override {
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
      window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoMove;
      window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen) ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    ImGui::End();
    ImGui::Begin("Scene");
    ImVec2 scene_size = ImGui::GetContentRegionAvail();
    if (scene_size.x != fbo_color_attachment_create_info_.width ||
        scene_size.y != fbo_color_attachment_create_info_.height) {
      fbo_color_attachment_create_info_.width = scene_size.x;
      fbo_color_attachment_create_info_.height = scene_size.y;
      rhi_.CreateTexture(fbo_color_attachment_create_info_, t_fbo_color_attachment_);
      auto attach_color_attachment_info = nanoR::RHIAttachColorAttachmentInfoOpenGL{};
      attach_color_attachment_info.level = 0;
      rhi_.AttachColorAttachment(attach_color_attachment_info, fbo_.get(), t_fbo_color_attachment_.get());
    }
    ImGui::Image(
        reinterpret_cast<void*>((dynamic_cast<nanoR::RHITextureOpenGL*>(t_fbo_color_attachment_.get())->id)),
        scene_size, {0, 1}, {1, 0}
    );
    ImGui::End();
    ImGui::Begin("Input");
    ImGui::Text("%X", nanoR::GlobalContext::Instance().input_system->control_commad);
    ImGui::Text(
        "x_offset: %f, y_offset: %f", nanoR::GlobalContext::Instance().input_system->cursor_x_offset,
        nanoR::GlobalContext::Instance().input_system->cursor_x_offset
    );
    ImGui::End();
  }

 private:
  std::shared_ptr<nanoR::RHIShaderProgram> shader_program_;
  std::shared_ptr<nanoR::Scene> scene_;
  nanoR::OpenGLMesh mesh_;
  nanoR::RHITextureCreateInfoOpenGL fbo_color_attachment_create_info_;
  std::shared_ptr<nanoR::RHITexture> t_fbo_color_attachment_;

  std::shared_ptr<nanoR::RHIFramebuffer> fbo_;
  nanoR::RHIOpenGL rhi_;

  nanoR::Camera<nanoR::CameraType::kPersp> main_camera_{{1, 0, 5}, {0, 0, 0}};
};

class InputLayer : public nanoR::Layer {
 public:
  InputLayer(std::string const& name) : Layer(name) {}
  auto OnEvent(std::shared_ptr<nanoR::Event> const& event) -> void override {
    if (event->GetType() == nanoR::EventType::kKeyDown) {
      auto key_event = dynamic_cast<nanoR::KeyDownEvent*>(event.get());
    }
  }
};

class Sandbox : public nanoR::ApplicationOpenGL {
 public:
  Sandbox() = default;
};

auto main() -> int {
  std::unique_ptr<Sandbox> sandbox = std::make_unique<Sandbox>();
  auto editor_layer = std::make_shared<EditorLayer>("editor_layer");
  auto input_layer = std::make_shared<InputLayer>("input layer");
  sandbox->PushLayer(editor_layer);
  sandbox->PushLayer(input_layer);
  LOG_TRACE("{}\n", sandbox->GetLayerStack().ToString());

  sandbox->Run();
  return 0;
}
