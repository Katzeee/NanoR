#pragma once
#include "scene/component.hpp"
#include <pico_libs/ecs/component.hpp>
#include <pico_libs/ecs/entity.hpp>
#include <pico_libs/mpl/type_list.hpp>

namespace nanoR {

class Resource {
public:
  Resource();
  Resource(std::string_view name);

  template <typename T, typename... Args>
  auto AddComponent(Args... args) -> T * {
    constexpr auto type_index = xac::mpl::index_of_v<T, Settings::ComponentList>;
    if (components_mask_.test(type_index)) {
      LOG_ERROR("Already has component of this type: {}\n", type_index);
      return nullptr;
    }
    components_mask_.set(type_index);
    auto &component = std::get<type_index>(components_);
    component = T(std::forward(args)...);
    return &component;
  }

  template <typename T>
  auto GetComponent() -> T * {
    constexpr auto type_index = xac::mpl::index_of_v<T, Settings::ComponentList>;
    if (components_mask_.test(type_index)) {
      return &std::get<type_index>(components_);
    }
    return nullptr;
  }

private:
  xac::mpl::rename<std::tuple, Settings::ComponentList> components_;
  std::bitset<Settings::ComponentList::size> components_mask_;
};
} // namespace nanoR