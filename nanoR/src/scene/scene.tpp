#include "entity.hpp"
#include "scene.hpp"

namespace nanoR {

// HINT: not a template function but defined in tpp means will be include into other cpps
inline auto Scene::CreateEntity() -> Entity {
  auto e = world_.create();
  auto c_t = world_.assign<TransformComponent>(e);
  auto c_n = world_.assign<NameComponent>(e);
  return Entity{e, this};
}
template <typename T>
auto Scene::Get(const Entity& e) -> xac::ecs::ComponentHandle<Settings, T> {
  return world_.get<T>(e.id_);
}

template <typename... Args>
auto Scene::View() {
  return world_.view<Args...>();
}

}  // namespace nanoR