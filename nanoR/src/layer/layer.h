#pragma once
#include "nanorpch.h"

namespace nanoR {
class Layer {
 public:
  Layer(std::string name) : name_(name) {}
  virtual auto OnAttach() -> void = 0;
  virtual auto Tick() -> void = 0;
  virtual auto OnDetach() -> void = 0;
  auto GetName() const -> std::string {
    return name_;
  }

 protected:
  std::string name_;
};
}  // namespace nanoR