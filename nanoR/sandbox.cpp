#include <memory>

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
    auto texture_create_info = nanoR::RHITextureCreateInfoOpenGL{};
    texture_create_info.width = 900;
    texture_create_info.height = 1600;
    texture_create_info.internal_format = GL_RGB8;
    texture_create_info.format = GL_RGB;
    texture_create_info.levels = 1;
    texture_create_info.target = GL_TEXTURE_2D;
    texture_create_info.type = GL_UNSIGNED_BYTE;
    texture_create_info.data = nullptr;
    texture_create_info.parameteri.push_back({GL_TEXTURE_MIN_FILTER, GL_LINEAR});
    texture_create_info.parameteri.push_back({GL_TEXTURE_MAG_FILTER, GL_LINEAR});
    texture_create_info.parameteri.push_back({GL_TEXTURE_WRAP_S, GL_REPEAT});
    texture_create_info.parameteri.push_back({GL_TEXTURE_WRAP_T, GL_REPEAT});
    rhi_.CreateTexture(texture_create_info, t_a_);
    auto attach_color_attachment_info = nanoR::RHIAttachColorAttachmentInfoOpenGL{};
    attach_color_attachment_info.level = 0;
    rhi_.AttachColorAttachment(attach_color_attachment_info, fbo_.get(), t_a_.get());

    const char* vert_shader_src = R"(#version 450
                                     layout(location = 0) in vec3 position;
                                     layout(location = 2) in vec2 texcoord;
                                     void main() {
                                       gl_Position = vec4(position, 1);
                                     }
                                    )";
    const char* frag_shader_src = R"(#version 450
                                     out vec4 FragColor;
                                     void main() {
                                       FragColor = vec4(1, 0, 1, 1);
                                     }
                                    )";
    std::shared_ptr<nanoR::RHIShaderModule> vert_shader;
    std::shared_ptr<nanoR::RHIShaderModule> frag_shader;
    auto shader_module_create_info = nanoR::RHIShaderModuleCreateInfoOpenGL{};
    shader_module_create_info.type = GL_VERTEX_SHADER;
    shader_module_create_info.src = vert_shader_src;
    rhi_.CreateShaderModule(shader_module_create_info, vert_shader);
    shader_module_create_info.type = GL_FRAGMENT_SHADER;
    shader_module_create_info.src = frag_shader_src;
    rhi_.CreateShaderModule(shader_module_create_info, frag_shader);
    auto shader_program_create_info = nanoR::RHIShaderProgramCreateInfoOpenGL{};
    shader_program_create_info.shaders.push_back(vert_shader);
    shader_program_create_info.shaders.push_back(frag_shader);
    rhi_.CreateShaderProgram(shader_program_create_info, shader_program_);
  }
  auto Tick(uint64_t delta_time) -> void override {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    rhi_.Draw(mesh_.vao.get(), shader_program_.get(), fbo_.get());
    // rhi_.Draw(mesh_.vao.get(), shader_program_.get(), std::nullopt);
  }
  auto OnDetach() -> void override {}
  auto OnEvent(nanoR::Event& event) -> void override {}
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
    // ImVec2 scene_size = ImGui::GetContentRegionAvail();
    ImGui::End();
    ImGui::Begin("Scene");
    ImGui::Image(
        reinterpret_cast<void*>((dynamic_cast<nanoR::RHITextureOpenGL*>(t_a_.get())->id)), {256, 256}, {0, 1}, {1, 0}
    );
    ImGui::End();
  }

 private:
  std::shared_ptr<nanoR::RHIShaderProgram> shader_program_;
  std::shared_ptr<nanoR::Scene> scene_;
  nanoR::OpenGLMesh mesh_;

  std::shared_ptr<nanoR::RHIFramebuffer> fbo_;
  std::shared_ptr<nanoR::RHITexture> t_a_;
  nanoR::RHIOpenGL rhi_;
};

class Sandbox : public nanoR::ApplicationOpenGL {
 public:
  Sandbox() = default;
};

auto main() -> int {
  std::unique_ptr<Sandbox> sandbox = std::make_unique<Sandbox>();
  std::shared_ptr<EditorLayer> test_layer1 = std::make_shared<EditorLayer>("test_layer1");
  sandbox->PushLayer(test_layer1);
  LOG_TRACE("{}\n", sandbox->GetLayerStack().ToString());

  sandbox->Run();

  return 0;
}
