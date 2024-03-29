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

#include "shader_compiler/shader_compiler.hpp"
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
    auto queue_indices = FindQueueFamilies(physical_device_);
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<int> unique_queue_families = {queue_indices.graphics_family, queue_indices.present_family};
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
    vkGetDeviceQueue(device_, queue_indices.graphics_family, 0, &graphics_queue_);
    vkGetDeviceQueue(device_, queue_indices.present_family, 0, &present_queue_);
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
        static_cast<uint32_t>(queue_indices.graphics_family), static_cast<uint32_t>(queue_indices.present_family)
    };
    if (queue_indices.graphics_family != queue_indices.present_family) {
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

#pragma region CREATE SWAPCHAIN IMAGE VIEW
    swapchain_image_views_.resize(swapchain_images_.size());
    for (size_t i = 0; i < swapchain_images_.size(); ++i) {
      VkImageViewCreateInfo image_view_create_info{};
      image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      image_view_create_info.image = swapchain_images_[i];
      image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
      image_view_create_info.format = surface_format.format;
      image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      image_view_create_info.subresourceRange.baseMipLevel = 0;
      image_view_create_info.subresourceRange.levelCount = 1;
      image_view_create_info.subresourceRange.baseArrayLayer = 0;
      image_view_create_info.subresourceRange.layerCount = 1;
      if (vkCreateImageView(device_, &image_view_create_info, nullptr, &swapchain_image_views_[i]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image views");
      }
    }
#pragma endregion

#pragma region CREATE RENDER PASS
    VkAttachmentDescription color_attachment{};
    color_attachment.format = surface_format.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    VkAttachmentReference color_attachment_reference{};
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass_desc{};
    subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_desc.colorAttachmentCount = 1;
    subpass_desc.pColorAttachments = &color_attachment_reference;

    VkRenderPassCreateInfo render_pass_create_info{};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &color_attachment;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass_desc;
    VkSubpassDependency subpass_dependency{};
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies = &subpass_dependency;
    if (vkCreateRenderPass(device_, &render_pass_create_info, nullptr, &render_pass_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create render pass");
    }
#pragma endregion

#pragma region CREATE GRAPHICS PIPELINE
    auto vert_shader_code = CompileShader("../25-vulkan-start/shader/triangle.vert.glsl", 1);
    auto frag_shader_code = CompileShader("../25-vulkan-start/shader/triangle.frag.glsl", 2);
    auto CreateShaderModule = [&](const std::vector<uint32_t>& shader_code) {
      VkShaderModuleCreateInfo shader_create_info{};
      shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      shader_create_info.codeSize = shader_code.size() * 4;
      shader_create_info.pCode = shader_code.data();
      VkShaderModule shader;
      if (vkCreateShaderModule(device_, &shader_create_info, nullptr, &shader) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
      }
      return shader;
    };
    vert_shader_ = CreateShaderModule(vert_shader_code);
    frag_shader_ = CreateShaderModule(frag_shader_code);
    VkPipelineShaderStageCreateInfo vert_shader_stage_create_info{};
    vert_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_create_info.module = vert_shader_;
    vert_shader_stage_create_info.pName = "main";
    VkPipelineShaderStageCreateInfo frag_shader_stage_create_info{};
    frag_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_create_info.module = frag_shader_;
    frag_shader_stage_create_info.pName = "main";
    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_create_info, frag_shader_stage_create_info};

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.pVertexBindingDescriptions = nullptr;
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = swapchain_extent_.width;
    viewport.height = swapchain_extent_.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain_extent_;
    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer_state{};
    rasterizer_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_state.depthClampEnable = VK_FALSE;
    rasterizer_state.rasterizerDiscardEnable = VK_FALSE;
    rasterizer_state.lineWidth = 1.0f;
    rasterizer_state.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer_state.depthBiasEnable = VK_FALSE;
    rasterizer_state.depthBiasConstantFactor = 0.0f;
    rasterizer_state.depthBiasClamp = 0.0f;
    rasterizer_state.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling_state{};
    multisampling_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_state.sampleShadingEnable = VK_FALSE;
    multisampling_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_state.minSampleShading = 1.0f;
    multisampling_state.pSampleMask = nullptr;
    multisampling_state.alphaToCoverageEnable = VK_FALSE;
    multisampling_state.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    VkPipelineColorBlendStateCreateInfo color_blend_state{};
    color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state.logicOpEnable = VK_FALSE;
    color_blend_state.attachmentCount = 1;
    color_blend_state.pAttachments = &color_blend_attachment;

    VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH};
    VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
    dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_create_info.dynamicStateCount = 2;
    dynamic_state_create_info.pDynamicStates = dynamic_states;

    VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = 0;
    if (vkCreatePipelineLayout(device_, &pipeline_layout_create_info, nullptr, &pipeline_layout_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.stageCount = 2;
    pipeline_create_info.pStages = shader_stages;
    pipeline_create_info.pVertexInputState = &vertex_input_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly;
    pipeline_create_info.pViewportState = &viewport_state;
    pipeline_create_info.pRasterizationState = &rasterizer_state;
    pipeline_create_info.pMultisampleState = &multisampling_state;
    pipeline_create_info.pDepthStencilState = nullptr;
    pipeline_create_info.pColorBlendState = &color_blend_state;
    pipeline_create_info.pDynamicState = nullptr;
    pipeline_create_info.layout = pipeline_layout_;
    pipeline_create_info.renderPass = render_pass_;
    pipeline_create_info.subpass = 0;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex = -1;
    if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline_) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to create graphics pipline");
    }
#pragma endregion

#pragma region CREATE FRAMEBUFFERS
    swapchain_framebuffers_.resize(swapchain_image_views_.size());
    for (size_t i = 0; i < swapchain_image_views_.size(); ++i) {
      VkImageView attachments[] = {swapchain_image_views_[i]};
      VkFramebufferCreateInfo framebuffer_create_info{};
      framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebuffer_create_info.renderPass = render_pass_;
      framebuffer_create_info.attachmentCount = 1;
      framebuffer_create_info.pAttachments = attachments;
      framebuffer_create_info.width = swapchain_extent_.width;
      framebuffer_create_info.height = swapchain_extent_.height;
      framebuffer_create_info.layers = 1;
      if (vkCreateFramebuffer(device_, &framebuffer_create_info, nullptr, &swapchain_framebuffers_[i]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create framebuffer");
      }
    }
#pragma endregion

#pragma region CREATE COMMAND POOL
    // queue_family_indices
    VkCommandPoolCreateInfo command_pool_create_info{};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.queueFamilyIndex = queue_indices.graphics_family;
    command_pool_create_info.flags = 0;
    if (vkCreateCommandPool(device_, &command_pool_create_info, nullptr, &command_pool_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create command pool");
    }
#pragma endregion

#pragma region CREATE COMMAND BUFFERS
    command_buffers_.resize(swapchain_framebuffers_.size());
    VkCommandBufferAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = command_pool_;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandBufferCount = command_buffers_.size();
    if (vkAllocateCommandBuffers(device_, &allocate_info, command_buffers_.data()) != VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate command buffer");
    }
#pragma endregion

#pragma region CREATE SEMAPHORES
    VkSemaphoreCreateInfo semaphore_create_info{};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(device_, &semaphore_create_info, nullptr, &image_available_semaphore_) != VK_SUCCESS ||
        vkCreateSemaphore(device_, &semaphore_create_info, nullptr, &render_finished_semaphore_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create semaphores");
    }
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
    for (auto framebuffer : swapchain_framebuffers_) {
      vkDestroyFramebuffer(device_, framebuffer, nullptr);
    }
    for (auto image_view : swapchain_image_views_) {
      vkDestroyImageView(device_, image_view, nullptr);
    }
    vkDestroySemaphore(device_, render_finished_semaphore_, nullptr);
    vkDestroySemaphore(device_, image_available_semaphore_, nullptr);
    vkDestroyCommandPool(device_, command_pool_, nullptr);
    vkDestroyPipeline(device_, pipeline_, nullptr);
    vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
    vkDestroyRenderPass(device_, render_pass_, nullptr);
    vkDestroyShaderModule(device_, vert_shader_, nullptr);
    vkDestroyShaderModule(device_, frag_shader_, nullptr);
    vkDestroySwapchainKHR(device_, swapchain_, nullptr);
    vkDestroyDevice(device_, nullptr);
    vkDestroySurfaceKHR(instance_, surface_, nullptr);
    vkDestroyInstance(instance_, nullptr);
  }

  void Draw() {
#pragma region RECORED COMMAND BUFFER HELPER
    auto RecordCommandBuffer = [&](VkCommandBuffer command_buffer, uint32_t image_index) {
      VkCommandBufferBeginInfo command_buffer_begin_info{};
      command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
      command_buffer_begin_info.pInheritanceInfo = nullptr;
      if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
      }
      VkRenderPassBeginInfo render_pass_begin_info{};
      render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      render_pass_begin_info.renderPass = render_pass_;
      render_pass_begin_info.framebuffer = swapchain_framebuffers_[image_index];
      render_pass_begin_info.renderArea.offset = {0, 0};
      render_pass_begin_info.renderArea.extent = swapchain_extent_;
      VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
      render_pass_begin_info.clearValueCount = 1;
      render_pass_begin_info.pClearValues = &clear_color;

      vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
      vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = swapchain_extent_.width;
      viewport.height = swapchain_extent_.height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(command_buffer, 0, 1, &viewport);
      VkRect2D scissor{};
      scissor.offset = {0, 0};
      scissor.extent = swapchain_extent_;
      vkCmdSetScissor(command_buffer, 0, 1, &scissor);
      vkCmdDraw(command_buffer, 3, 1, 0, 0);
      vkCmdEndRenderPass(command_buffer);
      if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer");
      }
    };
#pragma endregion
    uint32_t image_index;
    vkAcquireNextImageKHR(
        device_, swapchain_, std::numeric_limits<uint64_t>::max(), image_available_semaphore_, VK_NULL_HANDLE,
        &image_index
    );
    vkResetCommandBuffer(command_buffers_[image_index], /*VkCommandBufferResetFlagBits*/ 0);
    RecordCommandBuffer(command_buffers_[image_index], image_index);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wati_semaphores[] = {image_available_semaphore_};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wati_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffers_[image_index];

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffers_[image_index];

    VkSemaphore signal_semaphores[] = {render_finished_semaphore_};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;
    if (vkQueueSubmit(graphics_queue_, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
      throw std::runtime_error("Failed to submit draw command buffer");
    }
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    VkSwapchainKHR swapchains[] = {swapchain_};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;
    vkQueuePresentKHR(present_queue_, &present_info);
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
  std::vector<VkImageView> swapchain_image_views_;
  VkShaderModule vert_shader_;
  VkShaderModule frag_shader_;
  VkRenderPass render_pass_;
  VkPipelineLayout pipeline_layout_;
  VkPipeline pipeline_;
  std::vector<VkFramebuffer> swapchain_framebuffers_;
  VkCommandPool command_pool_;
  std::vector<VkCommandBuffer> command_buffers_;
  VkSemaphore image_available_semaphore_;
  VkSemaphore render_finished_semaphore_;
};

int main() {
  Application app;
  app.Init();
  while (!window->ShouldClose()) {
    window->SwapBuffers();
    app.Draw();
  }
  app.Destroy();
  return 0;
}