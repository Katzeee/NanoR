#include <memory>

#include "nanor.h"

class TestLayer : public nanoR::Layer {
 public:
  TestLayer(std::string name) : Layer(name) {}

  auto OnAttach() -> void override {}
  auto Tick() -> void override {}
  auto OnDetach() -> void override {}
};

class Sandbox : public nanoR::Application {
 public:
  Sandbox() = default;
  auto Init() -> void override {
    Application::Init();
  }
};

int main() {
  std::unique_ptr<Sandbox> sandbox = std::make_unique<Sandbox>();
  std::shared_ptr<TestLayer> test_layer1 = std::make_shared<TestLayer>("test_layer1");
  std::shared_ptr<TestLayer> test_layer2 = std::make_shared<TestLayer>("test_layer2");
  std::shared_ptr<TestLayer> test_overlay_layer = std::make_shared<TestLayer>("test_overlay_layer");
  sandbox->PushLayer(test_layer1);
  LOG_TRACE("{}\n", sandbox->GetLayerStack().ToString());
  sandbox->PushOverlayLayer(test_overlay_layer);
  LOG_TRACE("{}\n", sandbox->GetLayerStack().ToString());
  sandbox->PushLayer(test_layer2);
  LOG_TRACE("{}\n", sandbox->GetLayerStack().ToString());

  sandbox->Run();

  return 0;
}
