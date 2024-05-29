#include "scene.hpp"
#include "asset/asset_loader.h"


namespace nanoR {

// HINT: not a template function but defined in tpp means will be include into other cpps, so use inline
auto Scene::CreateEntity() -> Entity {
  auto e = world_.create();
  auto c_t = world_.assign<TransformComponent>(e);
  auto c_n = world_.assign<NameComponent>(e);
  return Entity{e, this};
}

auto Scene::CreatePointLight() -> Entity {
  auto *light = new Light{glm::vec3{1, 1, 1}, 100};
  auto point_light = CreateEntity();
  auto c_trans = point_light.GetComponenet<TransformComponent>();
  c_trans->position = {6, 5, 4};
  auto c_name = point_light.GetComponenet<NameComponent>();
  c_name->name = "Pointlight";
  auto c_light = point_light.AddComponent<LightCompoenent>();
  c_light->light.reset(light);
  return point_light;
}

auto Scene::CreateCube() -> Entity {
  auto cube_model = AssetLoader::LoadModelFromFile("../resources/models/Cube/cube.obj");
  auto cube_go = CreateEntity();
  // auto cube_mesh = CreateMesh(cube_mesh_data.meshes_.at(0));
  // auto c_name = GetComponent<NameComponent>(cube_go);
  // c_name->name = "Cube";
  // auto c_mesh = cube_go.AddComponent<MeshComponent>();
  // // TODO: get from resource manager
  // c_mesh->mesh = std::make_shared<Mesh>(cube_mesh[0]);
  // auto c_mesh_renderer = cube_go.AddComponent<MeshRendererCompoenent>();
  // c_mesh_renderer->materials.emplace_back(std::make_shared<Material>());
  return cube_go;
}

} // namespace nanoR