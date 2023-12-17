#include "scene.hpp"

#include "entity.hpp"

namespace nanoR {

auto Scene::CreateEntity() -> Entity {
  auto e = world_.create();
  auto c = world_.assign<TransformComponent>(e, TransformComponent{});
  return Entity{e, this};
}

}  // namespace nanoR