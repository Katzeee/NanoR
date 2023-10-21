#include <memory>

#include "application/application.h"
#include "nanorpch.h"

int main() {
  std::unique_ptr<nanoR::Application> app = std::make_unique<nanoR::Application>();
  app->Run();

  return 0;
}
