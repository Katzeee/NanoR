#include "resource.h"
#include "scene/component.hpp"

namespace nanoR {

Resource::Resource(std::string_view name, std::string_view path)
    : name_(name), path_(path) {}

Resource::Resource(std::string_view path)
    : path_(path) {
  auto index = path.find_last_of('/');
  if (index != path.npos) {
    name_ = path.substr(index, path.length() - index);
  }
}

} // namespace nanoR