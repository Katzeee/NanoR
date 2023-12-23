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

inline auto Scene::CreateCube() -> Entity {
  auto cube_mesh_data = Model("../resources/models/Cube/cube.obj");
  auto cube = CreateEntity();
  auto cube_mesh = CreateMesh(cube_mesh_data.meshes_.at(0));
  auto c_name = GetComponent<NameComponent>(cube);
  c_name->name = "Cube";
  auto c_mesh = cube.AddComponent<MeshComponent>();
  c_mesh->mesh = cube_mesh;
  auto c_mesh_renderer = cube.AddComponent<MeshRendererCompoenent>();
  c_mesh_renderer->materials.emplace_back(std::make_shared<Material>());
  return cube;
}

inline auto Scene::CreatePointLight() -> Entity {
  auto* light = new Light{glm::vec3{1, 1, 1}, 100};
  auto point_light = CreateEntity();
  auto c_trans = point_light.GetComponenet<TransformComponent>();
  c_trans->position = {6, -5, 4};
  auto c_name = point_light.GetComponenet<NameComponent>();
  c_name->name = "Pointlight";
  auto c_light = point_light.AddComponent<LightCompoenent>();
  c_light->light.reset(light);
  return point_light;
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