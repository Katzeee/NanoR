#pragma once
#include "layer.h"
#include "platform/opengl/rhi_type_opengl.h"
#include "render/rhi.h"
#include "render/rhi_type.h"

namespace nanoR {
class UILayer final : public Layer {
 public:
  UILayer(std::string name) : Layer(name) {}
  auto Tick(uint64_t delta_time) -> void override final;
  // auto TickUI() -> void override final {}
  auto SetBlockEvent(bool block) -> void {
    passthough_event_ = !block;
  }

  auto OnAttach() -> void override;
  auto OnDetach() -> void override;
  auto OnEvent(std::shared_ptr<Event> const& event) -> bool override;
  auto GetSceneFramebuffer() -> RHIFramebuffer*;

 protected:
  bool passthough_event_ = true;
  std::shared_ptr<RHIFramebuffer> scene_framebuffer_;
  RHITextureCreateInfoOpenGL scene_color_attachment_create_info_;
  std::shared_ptr<nanoR::RHITexture> scene_color_attachment_;
  std::shared_ptr<RHI> rhi_;

  auto CreateSceneFramebuffer() -> void;
  auto Begin() -> void;
  auto DockSpace() -> void;
  auto Scene() -> void;
  auto Input() -> void;
  auto End() -> void;
};
}  // namespace nanoR