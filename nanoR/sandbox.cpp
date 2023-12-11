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
    rhi_.Draw(mesh_.vao, shader_program_);
  }
  auto OnDetach() -> void override {}
  auto OnEvent(nanoR::Event& event) -> void override {}
  auto TickUI() -> void override {
    // ImGui::Begin("Demo");
    ImGui::ShowDemoWindow();
    // ImGui::End();
  }

 private:
  std::shared_ptr<nanoR::RHIShaderProgram> shader_program_;
  std::shared_ptr<nanoR::Scene> scene_;
  nanoR::OpenGLMesh mesh_;
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
