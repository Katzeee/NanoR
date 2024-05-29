#include "entity.hpp"
#include "resource/resource_manager.h"

namespace nanoR {

// inline auto Scene::LoadSampleScene() -> Entity {
//   auto herta_mesh_data = Model("../resources/models/Herta/heita.obj");
//   for (auto i = 0; i < herta_mesh_data.meshes_.size(); ++i) {
//     auto herta = CreateEntity();
//     auto herta_mesh = CreateMesh(herta_mesh_data.meshes_[i]);
//     auto c_name = GetComponent<NameComponent>(herta);
//     c_name->name = "Herta" + std::to_string(i);
//     auto c_mesh = herta.AddComponent<MeshComponent>();
//     c_mesh->mesh = herta_mesh;
//     auto c_mesh_renderer = herta.AddComponent<MeshRendererCompoenent>();
//     c_mesh_renderer->materials.emplace_back(std::make_shared<Material>());
//   }
//   return {};
// }

template <typename T>
auto Scene::GetComponent(const Entity &e) -> xac::ecs::ComponentHandle<Settings, T> {
  return world_.get<T>(e.id_);
}

template <typename... Args>
auto Scene::View() {
  return world_.view<Args...>();
}

template <typename F>
auto Scene::Each(F &&f) -> void {
  world_.each([&](auto &&e, uint32_t i) {
    // HINT: this entity is a temp object
    auto entity = Entity{e.GetId(), this};
    std::invoke(f, entity, i);
  });
}

} // namespace nanoR