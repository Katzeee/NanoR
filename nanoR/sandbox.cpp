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
    nanoR::MeshData mesh_data;
    mesh_data.vertices.resize(3);
    mesh_data.vertices[0].position = {0.0, 0.5, 1.0};
    mesh_data.vertices[1].position = {-0.5, -0.5, 1.0};
    mesh_data.vertices[2].position = {0.5, -0.5, 1.0};
    mesh_data.indices = {0, 1, 2};
    mesh_ = nanoR::CreateMesh(&rhi_, mesh_data);
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
    nanoR::ShaderData shader_data =
        nanoR::ResourceManager::LoadShaderData("../nanoR/shader/common.vert.glsl", "../nanoR/shader/unlit.frag.glsl");
    std::shared_ptr<nanoR::RHIShaderModule> vert_shader;
    std::shared_ptr<nanoR::RHIShaderModule> frag_shader;
    auto shader_module_create_info = nanoR::RHIShaderModuleCreateInfoOpenGL{};
    shader_module_create_info.type = GL_VERTEX_SHADER;
    shader_module_create_info.src = shader_data.vs_src.c_str();
    rhi_.CreateShaderModule(shader_module_create_info, vert_shader);
    shader_module_create_info.type = GL_FRAGMENT_SHADER;
    shader_module_create_info.src = shader_data.fs_src.c_str();
    rhi_.CreateShaderModule(shader_module_create_info, frag_shader);
    auto shader_program_create_info = nanoR::RHIShaderProgramCreateInfoOpenGL{};
    shader_program_create_info.shaders.push_back(vert_shader);
    shader_program_create_info.shaders.push_back(frag_shader);
    rhi_.CreateShaderProgram(shader_program_create_info, shader_program_);
  }

  auto Tick(uint64_t delta_time) -> void override {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    dynamic_cast<nanoR::RHIShaderProgramOpenGL*>(shader_program_.get())->SetValue("View", glm::mat4{1});
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
    ImGui::Begin("Settings");
    ImGui::Text("%X", nanoR::GlobalContext::Instance().input_system->control_commad);
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

  nanoR::Camera<nanoR::CameraType::kPersp> main_camera_{{0, 0, -5}, {0, 0, 0}};
};

class InputLayer : public nanoR::Layer {
 public:
  InputLayer(std::string const& name) : Layer(name) {}
  auto OnEvent(std::shared_ptr<nanoR::Event> const& event) -> void override {
    if (event->GetType() == nanoR::EventType::kKeyDown) {
      auto key_event = dynamic_cast<nanoR::KeyDownEvent*>(event.get());
      LOG_TRACE("{}\n", key_event->key_code);
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
