#pragma once
#include <pico_libs/ecs/component.hpp>
#include <pico_libs/ecs/settings.hpp>

#include "light/light.h"
#include "nanorpch.h"
#include "render/material.h"
#include "render/rhi_type.h"

namespace nanoR {

class Mesh;
class Entity;
class Resource;

struct NameComponent {
  std::string name = "Object";
};

struct TransformComponent {
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  // glm::quat rotation;
  glm::vec3 rotation{0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f, 1.0f, 1.0f};

  TransformComponent *parent = nullptr;
  std::vector<TransformComponent *> children;
  std::variant<Entity *, Resource *> owner;

  auto GetModelMatrix() const -> glm::mat4 {
    auto model = glm::mat4{1};
    model = glm::translate(model, position);
    auto quat = glm::quat(rotation);
    model *= glm::toMat4(quat);
    model = glm::scale(model, scale);
    return model;
  }
};

struct SpriteComponent {
  std::shared_ptr<RHITexture> sprite;
};

struct MeshComponent {
  std::shared_ptr<Mesh> mesh;
};

struct LightCompoenent {
  std::shared_ptr<Light> light;
};

struct MeshRendererCompoenent {
  std::vector<std::shared_ptr<Material>> materials;
};

using Components =
    xac::mpl::type_list<NameComponent, TransformComponent, MeshComponent, LightCompoenent, MeshRendererCompoenent>;
using Settings = xac::ecs::Settings<Components>;

} // namespace nanoR