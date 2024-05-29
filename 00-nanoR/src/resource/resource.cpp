#include "resource.h"
#include "scene/component.hpp"

namespace nanoR {
Resource::Resource() {
  AddComponent<NameComponent>();
  AddComponent<TransformComponent>()->owner = this;
}

Resource::Resource(std::string_view name)
    : Resource() {
  GetComponent<NameComponent>()->name = name;
}

} // namespace nanoR