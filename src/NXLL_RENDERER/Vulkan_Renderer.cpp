#include "Vulkan_Renderer.h"

// Based on:
// - https://www.youtube.com/watch?v=vZcoW989I3I&list=PLA0dXqQjCx0RntJy1pqje9uHRF1Z5vZgA&index=2
// -

VulkanR::VulkanR(std::string app_name)
{
    std::vector<const char *> ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    std::vector<const char *> Extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(_WIN32)
        "VK_KHR_win32_surface",
#endif
#if defined(_APPLE_)
        "VK_KHR_macos_surface",
#endif
#if defined(__linux__)
        "VK_KHR_xcb_surface",
#endif
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,

    };

    m_Vulkan_AppInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = app_name.c_str(),
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
        .pEngineName = "NXLL",
        .engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
        .apiVersion = VK_API_VERSION_1_4,
    };
    m_Vulkan_Instance_Create_Info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &m_Vulkan_AppInfo,
        .enabledLayerCount = (u32)(Layers.size()),
        .ppEnabledLayerNames = Layers.data(),
        .enabledExtensionCount = (u32)(Extensions.size()),
        .ppEnabledExtensionNames = Extensions.data(),

    };

    VkResult res = vkCreateInstance(&m_Vulkan_Instance_Create_Info, nullptr, &m_Vulkan_Instance);
    assert(res == VkResult::VK_SUCCESS, "[ERROR_VULKAN]: NXLL failed to initialize VKInstance. VkResult is %d.\n File: %s, Line: %d", res, __FILE__, __LINE__);

    std::cout << "[INFO_VULKAN]: Vulkan Instance has been initialized succesfully.\n";
};

VulkanR::~VulkanR()
{
    std::cout << "Destruct it all\n";
};