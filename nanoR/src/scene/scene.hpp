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

  template <typename T>
  auto Get(const Entity &e) -> xac::ecs::ComponentHandle<Settings, T>;

  template <typename... Args>
  auto View();

 private:
  xac::ecs::World<Settings> world_;
};
}  // namespace nanoR

#include "scene.tpp"
