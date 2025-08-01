#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include "../NXLL_CORE/constants.h"
#include <iostream>

class VulkanR
{
public:
    VkInstance m_Vulkan_Instance = nullptr;
    VkApplicationInfo m_Vulkan_AppInfo;
    VkInstanceCreateInfo m_Vulkan_Instance_Create_Info;

    VulkanR(std::string app_name);
    ~VulkanR();
};