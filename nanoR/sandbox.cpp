#include <memory>

#include "glad/glad.h"
#include "imgui.h"
#include "nanor.h"

class TestLayer : public nanoR::Layer {
 public:
  TestLayer(std::string name) : Layer(name) {}

  auto OnAttach() -> void override {
    float vertices[] = {0.0, 0.5, -0.5, 0.0, 0.5, 0.0};
    int indices[] = {0, 1, 2};
    auto buffer_create_info = nanoR::RHIBufferCreateInfoOpenGL{};
    buffer_create_info.size = sizeof(vertices);
    buffer_create_info.data = vertices;
    buffer_create_info.flags = 0;
    rhi_.CreateBuffer(buffer_create_info, vbo);
    rhi_.CreateVertexArray(vao);
    auto bind_vertex_buffer_info = nanoR::RHIBindVertexBufferInfoOpenGL{};
    bind_vertex_buffer_info.bind_index = 0;
    bind_vertex_buffer_info.attr_index = 0;
    bind_vertex_buffer_info.offset = 0;
    bind_vertex_buffer_info.stride = sizeof(float[2]);
    bind_vertex_buffer_info.normalized = GL_FALSE;
    bind_vertex_buffer_info.type = GL_FLOAT;
    bind_vertex_buffer_info.attr_size = 2;
    rhi_.BindVertexBuffer(bind_vertex_buffer_info, vao, vbo);
    buffer_create_info.size = sizeof(indices);
    buffer_create_info.data = indices;
    rhi_.CreateBuffer(buffer_create_info, ebo);
    auto bind_index_buffer_info = nanoR::RHIBindIndexBufferInfoOpenGL{};
    bind_index_buffer_info.count = 3;
    rhi_.BindIndexBuffer(bind_index_buffer_info, vao, ebo);

    const char* vert_shader_src = R"(#version 450
                                     layout(location = 0) in vec2 position;
                                     void main() {
                                       gl_Position = vec4(position, 1, 1);
                                     }
                                    )";
    const char* frag_shader_src = R"(#version 450
                                     out vec4 FragColor;
                                     void main() {
                                       FragColor = vec4(1, 0, 1, 1);
                                     }
                                    )" ;
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
    rhi_.CreateShaderProgram(shader_program_create_info, shader_program);
  }
  auto Tick() -> void override {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    rhi_.Draw(vao, shader_program);
  }
  auto OnDetach() -> void override {}
  auto OnEvent(nanoR::Event& event) -> void override {}
  auto TickUI() -> void override {
    // ImGui::Begin("Demo");
    ImGui::ShowDemoWindow();
    // ImGui::End();
  }

 private:
  std::shared_ptr<nanoR::RHIVertexArray> vao;
  std::shared_ptr<nanoR::RHIBuffer> vbo;
  std::shared_ptr<nanoR::RHIBuffer> ebo;
  std::shared_ptr<nanoR::RHIShaderProgram> shader_program;
  nanoR::RHIOpenGL rhi_;
};

class Sandbox : public nanoR::ApplicationOpenGL {
 public:
  Sandbox() = default;
};

int main() {
  std::unique_ptr<Sandbox> sandbox = std::make_unique<Sandbox>();
  std::shared_ptr<TestLayer> test_layer1 = std::make_shared<TestLayer>("test_layer1");
  sandbox->PushLayer(test_layer1);
  LOG_TRACE("{}\n", sandbox->GetLayerStack().ToString());

  sandbox->Run();

  return 0;
}
