#include "model.h"
#include "nanorpch.h"

namespace nanoR {

ModelPart::ModelPart(std::string_view name)
    : name_(name) {
  transform_.owner = this;
}

ModelPart::ModelPart()
    : ModelPart("") {
}

} // namespace nanoR