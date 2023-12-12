#pragma once
#include "layer/ui_layer.h"

namespace nanoR {
class UILayerGLFWOpenGL3 : public UILayer {
 public:
  UILayerGLFWOpenGL3(std::string name = "UI Layer") : UILayer(name) {}
  auto OnAttach() -> void override;
  auto OnDetach() -> void override;
  auto OnEvent(std::shared_ptr<Event> const& event) -> void override;
  auto Begin() -> void override;
  auto End() -> void override;
};
}  // namespace nanoR