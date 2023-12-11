#include "scene.hpp"

#include "entity.hpp"

namespace nanoR {

auto Scene::CreateEntity() -> Entity {
  auto id = world_.create();
  return {};
}

}  // namespace nanoR