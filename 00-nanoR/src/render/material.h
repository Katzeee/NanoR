#pragma once
#include "render/rhi.h"

namespace nanoR {

class Material {
 public:
  Material();
  Material(std::string_view shader_name);

  auto GetName() -> std::string_view;
  auto GetUniformDescs() -> std::map<std::string, UniformBufferDesc> &;
  auto PrepareUniforms(RHI *rhi) -> void;

 private:
  std::string shader_name_;
  std::map<std::string, UniformBufferDesc> uniform_descs;
};

}  // namespace nanoR