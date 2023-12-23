#pragma once
namespace nanoR {
class RHI;
class RHITexture;

class Material {
 public:
  Material();

  auto GetName() -> std::string_view;

  auto GetVec4(std::string_view name) -> glm::vec4 &;

  auto PrepareUniforms(RHI *rhi) -> void;

 private:
  std::string shader_name_;
  std::unordered_map<std::string, glm::vec4> vec4_storage_;
  std::unordered_map<std::string, std::shared_ptr<RHITexture>> texture_storage_;
};

}  // namespace nanoR