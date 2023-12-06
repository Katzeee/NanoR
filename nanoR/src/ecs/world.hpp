#pragma once
#include <assert.h>

#include <bitset>
#include <memory>
#include <vector>

#include "component.hpp"
#include "entity.hpp"
#include "mpu.hpp"
namespace xac::ecs {
template <typename TSettings>
class Entity;

template <typename TComponentList>
struct Settings {
  using ComponentList = TComponentList;
};

constexpr static uint32_t kInitSize = 200;

template <typename TSettings>
class World {
 public:
  using ComponentList = TSettings::ComponentList;
  template <typename... Args>
  using TupleOfVectors = std::tuple<std::vector<Args>...>;

 public:
  World();
  auto create() -> Entity<TSettings>;
  template <typename T, typename... Args>
  auto assign() -> std::shared_ptr<ComponentHandle>;

 private:
  auto prepare_create() -> void;

 private:
  mpu::rename<TupleOfVectors, ComponentList> components_pool_;
  std::vector<Entity<TSettings>> entities_;
  std::vector<uint32_t> entity_version_;
  static uint32_t entity_count_;
};

template <typename TSettings>
inline uint32_t World<TSettings>::entity_count_ = 0;

template <typename TSettings>
World<TSettings>::World() {
  entities_.resize(kInitSize);
  entity_version_.resize(kInitSize);
}

template <typename TSettings>
auto World<TSettings>::create() -> Entity<TSettings> {
  prepare_create();
  auto id = typename Entity<TSettings>::Id(entity_count_, ++entity_version_[entity_count_]);
  entities_.emplace(entities_.begin() + entity_count_, id, this);
  return entities_[entity_count_];
}

template <typename TSettings>
template <typename T, typename... Args>
auto World<TSettings>::assign() -> std::shared_ptr<ComponentHandle> {
  return nullptr;
}

template <typename TSettings>
auto World<TSettings>::prepare_create() -> void {
  assert(entities_.size() == entity_version_.size());
  if (entity_count_ >= entities_.size()) {
    entities_.resize(++entity_count_);  // HINT: must be resize, because you will use index
    entity_version_.resize(entity_count_);
  }
}

}  // namespace xac::ecs