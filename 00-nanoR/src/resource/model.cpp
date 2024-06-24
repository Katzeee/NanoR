#include "model.h"
#include "nanorpch.h"
#include "scene/scene.hpp"

namespace nanoR {

ModelPart::ModelPart(std::string_view name)
    : name_(name) {
  transform_.owner = this;
}

ModelPart::ModelPart()
    : ModelPart("") {
}

auto Model::ToEntityInner(Scene *scene, ModelPart *node, Entity &entity) -> TransformComponent * {
  entity.GetComponenet<NameComponent>()->name = node->name_;
  auto transform = entity.GetComponenet<TransformComponent>();
  transform->position = node->transform_.position;
  transform->rotation = node->transform_.rotation;
  transform->scale = node->transform_.scale;
  transform->owner = &entity;
  auto mesh = entity.GetComponenet<MeshComponent>();
  mesh->mesh = node->mesh_.mesh;

  for (const auto child : node->transform_.children) {
    auto child_node = std::any_cast<ModelPart *>(child->owner);
    auto child_entity = scene->CreateEntity();
    // assign parent
    child_entity.GetComponenet<TransformComponent>()->parent = transform.get();
    transform->children.emplace_back(ToEntityInner(scene, child_node, child_entity));
  }
  return transform.get();
}

auto Model::ToEntity(Scene *scene) -> Entity {
  auto entity = scene->CreateEntity();
  ToEntityInner(scene, &root_, entity);
  return entity;
}

} // namespace nanoR