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
  auto CreateCube() -> Entity;
  auto CreatePointLight() -> Entity;

  template <typename T>
  auto GetComponent(const Entity &e) -> xac::ecs::ComponentHandle<Settings, T>;

  template <typename... Args>
  auto View();

  template <typename F>
  auto Each(F &&f) -> void;

 private:
  xac::ecs::World<Settings> world_;
};
}  // namespace nanoR

#include "scene.tpp"
