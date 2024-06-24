#pragma once
#include <pico_libs/ecs/component.hpp>
#include <pico_libs/ecs/entity.hpp>

#include "component.hpp"

namespace nanoR {
class Scene;
class Entity {
  friend class Scene;

public:
  Entity() = default;
  Entity(xac::ecs::Entity<Settings>::Id id, Scene *scene)
      : id_(id), scene_(scene) {}
  operator bool();

  template <typename T, typename... Args>
  auto AddComponent(Args &&...args) -> xac::ecs::ComponentHandle<Settings, T>;

  template <typename T>
  auto GetComponenet() -> xac::ecs::ComponentHandle<Settings, T>;

private:
  xac::ecs::Entity<Settings>::Id id_;
  Scene *scene_ = nullptr;
};

} // namespace nanoR

#include "entity.tpp"