#pragma once

#include "resource.h"
#include "scene/component.hpp"

namespace nanoR {

class ModelPart {
public:
  friend class AssetLoader;
  friend class Model;

  ModelPart();
  ModelPart(std::string_view name);

private:
  std::string name_;
  TransformComponent transform_;
  MeshComponent mesh_;
};

class Entity;
class Scene;

class Model : Resource {
public:
  friend class AssetLoader;
  Model(std::string_view path)
      : Resource(path) {}

  auto ToEntity(Scene *scene) -> Entity;

private:
  auto ToEntityInner(Scene *scene, ModelPart *node, Entity &parent) -> TransformComponent *;

  ModelPart root_;
};

} // namespace nanoR