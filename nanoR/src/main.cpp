#include <memory>

#include "logger/logger.h"
#include "nanorpch.h"

int main() {
  std::unique_ptr<nanoR::Window> window = std::make_unique<nanoR::LinuxWindow>();
  while (true) {
    window->Tick();
  }

  return 0;
}
