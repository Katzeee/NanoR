#pragma once
#include <pico_libs/ecs/world.hpp>

#include "component.hpp"

namespace nanoR {
class Entity;

class Scene {
  friend class Entity;

 public:
  Scene() = default;
  ~Scene() = default;
  auto CreateEntity() -> Entity;

 private:
  xac::ecs::World<Settings> world_;
};
}  // namespace nanoR
