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

class Model : Resource {
public:
  friend class AssetLoader;
  Model(std::string_view path)
      : Resource(path) {}

private:
  ModelPart root_;
};

} // namespace nanoR