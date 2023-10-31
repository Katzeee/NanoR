#include <memory>

#include "glad.h"
#include "imgui.h"
#include "nanor.h"

class TestLayer : public nanoR::Layer {
 public:
  TestLayer(std::string name) : Layer(name) {}

  auto OnAttach() -> void override {}
  auto Tick() -> void override {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
  }
  auto OnDetach() -> void override {}
  auto OnEvent(nanoR::Event& event) -> void override {}
  auto TickUI() -> void override {
    // ImGui::Begin("Demo");
    ImGui::ShowDemoWindow();
    // ImGui::End();
  }
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
