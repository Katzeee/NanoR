#pragma once
#include <pico_libs/ecs/component.hpp>
#include <pico_libs/ecs/entity.hpp>

#include "component.hpp"
#include "scene.hpp"

namespace nanoR {
class Scene;
class Entity {
 public:
  Entity() = delete;
  Entity(xac::ecs::Entity<Settings>::Id id, Scene* scene) : id_(id), scene_(scene) {}

  template <typename T, typename... Args>
  auto AddComponent(Args&&... args) -> xac::ecs::ComponentHandle<Settings, T> {
    return scene_->world_.assign<T>(id_, std::forward<Args>(args)...);
  }
  template <typename T>
  auto GetComponenet() -> xac::ecs::ComponentHandle<Settings, T> {
    return scene_->world_.get<T>(id_);
  }

 private:
  xac::ecs::Entity<Settings>::Id id_;
  Scene* scene_;
};

}  // namespace nanoR