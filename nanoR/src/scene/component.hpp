#pragma once
#include <pico_libs/ecs/settings.hpp>

#include "nanorpch.h"
#include "platform/opengl/render_resource_opengl.h"
#include "render/rhi_type.h"

namespace nanoR {

struct NameComponent {
  std::string name;
};

struct TransformComponent {
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  // glm::quat rotation;
  glm::vec3 rotation;
  glm::vec3 scale{1.0f, 1.0f, 1.0f};

  auto GetModelMatrix() -> glm::mat4 {
    auto model = glm::mat4{1};
    model = glm::scale(model, scale);
    auto quat = glm::quat(rotation);
    // model = glm::rotate(model, rotation);
    model = glm::toMat4(quat) * model;
    model = glm::translate(model, position);
    return model;
  }
};

struct SpriteComponent {
  std::shared_ptr<RHITexture> sprite;
};

struct MeshComponent {
  std::shared_ptr<OpenGLMesh> mesh;
};

struct MaterialCompoenent {};

using Components = xac::mpl::type_list<NameComponent, TransformComponent, SpriteComponent, MeshComponent>;
using Settings = xac::ecs::Settings<Components>;

}  // namespace nanoR