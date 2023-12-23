#pragma once
namespace nanoR {
class RHI;
class RHITexture;

class Material {
 public:
  Material();
  Material(std::string_view shader_name);

  auto GetName() -> std::string_view;
  auto SetVec4(std::string_view name, glm::vec4 const &value) -> void;
  auto GetVec4(std::string_view name) -> glm::vec4 &;
  auto SetTexture(std::string_view name, std::shared_ptr<RHITexture> const &texture) -> void;
  auto GetTexture(std::string_view name) -> RHITexture *;
  auto PrepareUniforms(RHI *rhi) -> void;

 private:
  std::string shader_name_;
  std::unordered_map<std::string, glm::vec4> vec4_storage_;
  std::unordered_map<std::string, std::shared_ptr<RHITexture>> texture_storage_;
};

}  // namespace nanoR