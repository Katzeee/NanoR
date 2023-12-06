#pragma once
#include "mpu.hpp"

namespace xac::ecs {
template <typename TComponentList>
struct Settings {
  using ComponentList = TComponentList;
  template <typename T>
  constexpr static auto has_component() -> bool {
    return mpu::is_in_type_list<T, ComponentList>::value;
  }
};

}  // namespace xac::ecs