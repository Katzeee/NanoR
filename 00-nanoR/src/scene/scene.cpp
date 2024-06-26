#include "scene.hpp"
#include "asset/asset_loader.h"
#include "global/global_context.h"
#include "resource/model.h"

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
  // auto cube_model = AssetLoader::LoadModelFromFile("../resources/models/Cube/cube.obj");
  auto cube_model = AssetLoader::LoadModelFromFile("../resources/models/Cube/cube.obj");
  // // TODO: get from resource manager
  auto cube_go = cube_model.ToEntity(this);

  cube_go.GetComponenet<MeshRendererComponent>()->materials.emplace_back(std::make_shared<Material>("unlit"));
  return cube_go;
}

} // namespace nanoR