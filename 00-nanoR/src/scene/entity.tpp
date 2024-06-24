#pragma once

#include "entity.hpp"
#include "scene.hpp"

namespace nanoR {

inline Entity::operator bool() {
  return scene_;
}

template <typename T, typename... Args>
auto Entity::AddComponent(Args &&...args) -> xac::ecs::ComponentHandle<Settings, T> {
  return scene_->world_.assign<T>(id_, std::forward<Args>(args)...);
}

template <typename T>
auto Entity::GetComponenet() -> xac::ecs::ComponentHandle<Settings, T> {
  if (scene_->world_.has<T>(id_)) {
    return scene_->world_.get<T>(id_);
  } else {
    return AddComponent<T>();
  }
}
} // namespace nanoR