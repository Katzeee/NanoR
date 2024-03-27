#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vulkan/vulkan.h>

#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

#include "window/window.hpp"

const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

using namespace xac;

std::unique_ptr<Window> window = std::make_unique<Window>();

class Application {
 public:
  void Run() {}

  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
  ) {
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;  // only return false when test validate the validation layer itself
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
    if (enable_validation_layers_ && !CheckValidationLayerSupport()) {
      throw std::runtime_error("Failed to find validation layer");
    }
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
    if (enable_validation_layers_) {
      extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    instance_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instance_create_info.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
    if (enable_validation_layers_) {
      instance_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
      instance_create_info.ppEnabledLayerNames = validation_layers.data();
      debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
      debug_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
      debug_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
      debug_messenger_create_info.pfnUserCallback = DebugCallBack;
      debug_messenger_create_info.pUserData = nullptr;
      instance_create_info.pNext = &debug_messenger_create_info;
    } else {
      instance_create_info.enabledLayerCount = 0;
      instance_create_info.pNext = nullptr;
    }

    if (VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance_); result != VK_SUCCESS) {
      throw std::runtime_error("Failed to create vk instance");
    }
#pragma endregion

#pragma region SETUP DEBUG CALLBACK
    auto CreateDebugUtilsMessengerEXT = [&]() {
      auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT")
      );
      if (func == nullptr) {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
      }
      return func(instance_, &debug_messenger_create_info, nullptr, &debug_messenger_);
    };
    if (CreateDebugUtilsMessengerEXT() != VK_SUCCESS) {
      throw std::runtime_error("Failed to setup debug callback");
    }
#pragma endregion

#pragma region SURFACE
    if (glfwCreateWindowSurface(instance_, window->GetRawWindow(), nullptr, &surface_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create window surface");
    }
#pragma endregion

#pragma region QUEUE FAMILY HELPERS
    struct QueueFamilyIndices {
      int graphics_family = -1;
      int present_family = -1;
      bool IsComplete() {
        return graphics_family >= 0 && present_family >= 0;
      }
    };
    auto FindQueueFamilies = [&](VkPhysicalDevice device) {
      QueueFamilyIndices indices;
      uint32_t queue_family_count = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
      std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
      for (int i = 0; i < queue_family_count; i++) {
        if (queue_families[i].queueCount > 0 && queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
          indices.graphics_family = i;
        }
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);
        if (present_support) {
          indices.present_family = i;
        }
        if (indices.IsComplete()) {
          break;
        }
      }
      return indices;
    };
#pragma endregion

#pragma region SWAPCHAIN HELPERS
    struct SwapChainSupportDetails {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> present_modes;
    };

    auto QuerySwapChainSupport = [&](VkPhysicalDevice device) {
      SwapChainSupportDetails details;
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);
      uint32_t format_count;
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, nullptr);
      if (format_count != 0) {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &format_count, details.formats.data());
      }
      uint32_t present_mode_count;
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, nullptr);
      if (present_mode_count != 0) {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_mode_count, details.present_modes.data());
      }

      return details;
    };
#pragma endregion

#pragma region PHYSICAL DEVICE
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);
    if (device_count == 0) {
      throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

    auto IsDeviceSuitable = [&](VkPhysicalDevice device) {
      VkPhysicalDeviceProperties device_properties;
      VkPhysicalDeviceFeatures device_features;
      vkGetPhysicalDeviceProperties(device, &device_properties);
      vkGetPhysicalDeviceFeatures(device, &device_features);

      uint32_t extension_count;
      vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
      std::vector<VkExtensionProperties> available_extensions(extension_count);
      vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());
      std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());
      for (const auto& extension : available_extensions) {
        required_extensions.erase(extension.extensionName);
      }

      bool swap_chain_adequate = false;
      auto swap_chain_support = QuerySwapChainSupport(device);
      swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();

      return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && device_features.geometryShader &&
             FindQueueFamilies(device).IsComplete() && required_extensions.empty() && swap_chain_adequate;
    };
    for (const auto& device : devices) {
      if (IsDeviceSuitable(device)) {
        physical_device_ = device;
        break;
      }
    }
    if (physical_device_ == VK_NULL_HANDLE) {
      throw std::runtime_error("Failed to find a suitable GPU");
    }
#pragma endregion

#pragma region LOGICAL DEVICE
    auto indices = FindQueueFamilies(physical_device_);
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<int> unique_queue_families = {indices.graphics_family, indices.present_family};
    float queue_priority = 1.0f;
    for (int queue_family : unique_queue_families) {
      VkDeviceQueueCreateInfo queue_create_info{};
      queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_create_info.queueFamilyIndex = queue_family;
      queue_create_info.queueCount = 1;
      queue_create_info.pQueuePriorities = &queue_priority;
      queue_create_infos.emplace_back(std::move(queue_create_info));
    }
    VkPhysicalDeviceFeatures device_features{};
    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    device_create_info.pQueueCreateInfos = queue_create_infos.data();
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    device_create_info.ppEnabledExtensionNames = device_extensions.data();
    if (enable_validation_layers_) {
      device_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
      device_create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
      device_create_info.enabledLayerCount = 0;
    }
    if (vkCreateDevice(physical_device_, &device_create_info, nullptr, &device_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create logical device");
    }
    vkGetDeviceQueue(device_, indices.graphics_family, 0, &graphics_queue_);
    vkGetDeviceQueue(device_, indices.present_family, 0, &present_queue_);
#pragma endregion

#pragma region CREATE SWAPCHAIN
    auto swapchain_supports = QuerySwapChainSupport(physical_device_);
    auto ChooseSwapSurfaceFormat = [&]() {
      for (const auto& format : swapchain_supports.formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
          return format;
        }
      }
      return swapchain_supports.formats[0];
    };
    auto ChooseSwapPresentMode = [&]() {
      for (const auto& present_mode : swapchain_supports.present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
          return present_mode;
        }
      }
      return swapchain_supports.present_modes[0];
    };
    auto ChooseSwapExtent = [&]() {
      const auto& capabilities = swapchain_supports.capabilities;
      if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
      }
      int width, height;
      glfwGetFramebufferSize(window->GetRawWindow(), &width, &height);
      VkExtent2D extent{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
      extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
      extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
      return extent;
    };
    VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat();
    VkPresentModeKHR present_mode = ChooseSwapPresentMode();
    swapchain_extent_ = ChooseSwapExtent();
    uint32_t image_count = swapchain_supports.capabilities.minImageCount + 1;
    if (swapchain_supports.capabilities.maxImageCount > 0 &&
        image_count > swapchain_supports.capabilities.maxImageCount) {
      image_count = swapchain_supports.capabilities.maxImageCount;
    }
    VkSwapchainCreateInfoKHR swapchain_create_info{};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = surface_;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = surface_format.colorSpace;
    swapchain_create_info.imageExtent = swapchain_extent_;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_family_indices[] = {
        static_cast<uint32_t>(indices.graphics_family), static_cast<uint32_t>(indices.present_family)
    };
    if (indices.graphics_family != indices.present_family) {
      swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      swapchain_create_info.queueFamilyIndexCount = 2;
      swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
      swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    swapchain_create_info.preTransform = swapchain_supports.capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(device_, &swapchain_create_info, nullptr, &swapchain_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create swapchain");
    }
    vkGetSwapchainImagesKHR(device_, swapchain_, &image_count, nullptr);
    swapchain_images_.resize(image_count);
    vkGetSwapchainImagesKHR(device_, swapchain_, &image_count, swapchain_images_.data());
#pragma endregion
  }

  void Destroy() {
    auto DestroyDebugUtilsMessengerEXT = [&]() {
      auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT")
      );
      if (func != nullptr) {
        return func(instance_, debug_messenger_, nullptr);
      }
    };
    if (enable_validation_layers_) {
      DestroyDebugUtilsMessengerEXT();
    }
    vkDestroySwapchainKHR(device_, swapchain_, nullptr);
    vkDestroyDevice(device_, nullptr);
    vkDestroySurfaceKHR(instance_, surface_, nullptr);
    vkDestroyInstance(instance_, nullptr);
  }

 private:
  bool enable_validation_layers_ = true;
  VkInstance instance_;
  VkSurfaceKHR surface_;
  VkDebugUtilsMessengerEXT debug_messenger_;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_;
  VkQueue graphics_queue_;
  VkQueue present_queue_;
  VkSwapchainKHR swapchain_;
  std::vector<VkImage> swapchain_images_;
  VkExtent2D swapchain_extent_;
};

int main() {
  Application app;
  app.Init();
  while (!window->ShouldClose()) {
    window->SwapBuffers();
  }
  app.Destroy();
  return 0;
}