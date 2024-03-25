#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "window/window.hpp"

const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};

class Application {
 public:
  void Run() {}

  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCB(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
  ) {
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;  // only return false when test validate the validation layer itself
  }

  VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
      const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger
  ) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
      return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }

  void Init() {
#pragma region CREATE INSTANCE
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Vulkan";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    auto ListAvaliableExtensions = []() {
      uint32_t extension_count = 0;
      vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
      std::vector<VkExtensionProperties> extensions(extension_count);
      vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
      std::cout << "Available extensions:" << std::endl;
      for (const auto& extension : extensions) {
        std::cout << "\t" << extension.extensionName << std::endl;
      }
    };
    // ListAvaliableExtensions();
    auto CheckValidationLayerSupport = []() {
      uint32_t layer_count = 0;
      vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
      std::vector<VkLayerProperties> available_layers(layer_count);
      vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

      for (const auto& layer : validation_layers) {
        bool found = false;
        for (const auto& available_layer : available_layers) {
          if (strcmp(layer, available_layer.layerName) == 0) {
            found = true;
            break;
          }
        }
        if (!found) {
          return false;
        }
      }
      return true;
    };

    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;
    if (!CheckValidationLayerSupport()) {
      throw std::runtime_error("Failed to find validation layer");
    }
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    instance_create_info.ppEnabledLayerNames = validation_layers.data();
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instance_create_info.ppEnabledExtensionNames = extensions.data();
    instance_create_info.enabledExtensionCount = glfw_extension_count;
    instance_create_info.enabledLayerCount = 0;
    if (VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance_); result != VK_SUCCESS) {
      throw std::runtime_error("Failed to create vk instance");
    }
#pragma endregion
#pragma region SETUP DEBUG CALLBACK
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
    debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_create_info.pfnUserCallback = DebugCB;
    debug_messenger_create_info.pUserData = nullptr;
    // auto CreateDebugUtilsMessengerEXT = [&]() {
    //   auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
    //       vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT")
    //   );
    //   if (func == nullptr) {
    //     return VK_ERROR_EXTENSION_NOT_PRESENT;
    //   }
    //   return func(instance_, &debug_messenger_create_info, nullptr, &callback);
    // };
    if (CreateDebugUtilsMessengerEXT(instance_, &debug_messenger_create_info, nullptr, &debug_messenger_) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to setup debug callback");
    }
#pragma endregion
  }

  void Destroy() {
    vkDestroyInstance(instance_, nullptr);
  }

 private:
  VkInstance instance_;
  VkDebugUtilsMessengerEXT debug_messenger_;
};

using namespace xac;

int main() {
  std::unique_ptr<Window> window;
  Application app;
  app.Init();
  app.Destroy();
  return 0;
}