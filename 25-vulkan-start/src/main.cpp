#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

int main() {
  if (!glfwInit()) {
    return -1;
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  auto *window = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}