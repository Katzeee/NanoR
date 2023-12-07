#pragma once
#include "mpu.hpp"

namespace xac::ecs {
template <typename TComponentList>
struct Settings {
  using ComponentList = TComponentList;
  template <typename T>
  constexpr static auto has_component() -> bool {
    return mpu::contains<T, ComponentList>::value;
  }
};

}  // namespace xac::ecs