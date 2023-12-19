#include <utility>

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
auto Scene::GetComponent(const Entity& e) -> xac::ecs::ComponentHandle<Settings, T> {
  return world_.get<T>(e.id_);
}

template <typename... Args>
auto Scene::View() {
  return world_.view<Args...>();
}

template <typename F>
auto Scene::Each(F&& f) -> void {
  world_.each([&](auto&& e, uint32_t i) {
    // HINT: this entity is a temp object
    auto entity = Entity{e.GetId(), this};
    std::invoke(f, entity, i);
  });
}

}  // namespace nanoR