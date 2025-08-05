#include "V.h"

// Vulkan strange functions.

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void V::cleanupDebugUtilsMessengerEXT()
{
    DestroyDebugUtilsMessengerEXT(m_Vulkan_Instance, m_DebugMessenger, nullptr);
}

VkResult V::CreateLogicalDevice(bool isValidationOn, QueueFamilyIndices *indices)
{

    std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
    std::set<uint32_t> UniqueQueueFamilies = {
        indices->m_GraphicsFamily.value(),
        indices->m_PresentFamily.value(),
    };

    float QueuePriority = 1.0f;
    for (uint32_t QueueFamily : UniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo QueueCreateInfo = {};
        QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueCreateInfo.queueFamilyIndex = QueueFamily;
        QueueCreateInfo.queueCount = 1;
        QueueCreateInfo.pQueuePriorities = &QueuePriority;

        QueueCreateInfos.push_back(QueueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo DeviceCreateInfo = {};
    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(QueueCreateInfos.size());
    DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
    DeviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
    DeviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
    DeviceCreateInfo.enabledLayerCount = 0;

    if (isValidationOn)
    {
        DeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
        DeviceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
    }

    return vkCreateDevice(m_Physical_Device, &DeviceCreateInfo, nullptr, &m_Device);
}

// Implementation copie- no, i mean, based in:
// https://vulkan-tutorial.com/en/Drawing_a_triangle/Presentation/Swap_chain

std::pair<VkResult, uint32_t> V::CreateSwapChain(SwapChainSupportDetails *SC_Support_Details, VkSurfaceFormatKHR surfaceFormat,
                                                 VkExtent2D extent, QueueFamilyIndices *indices)
{
    // Selects the presentaion mode.
    VkPresentModeKHR presentMode = SC_Support_Details->ChoosePresentMode();

    uint32_t imageCount = SC_Support_Details->m_Capabilities.minImageCount + 1;
    bool boolean = SC_Support_Details->m_Capabilities.maxImageCount > 0 &&
                   imageCount > SC_Support_Details->m_Capabilities.maxImageCount;

    imageCount = (boolean) ? (SC_Support_Details->m_Capabilities.maxImageCount) : (imageCount);

    uint32_t QueueFamilyIndices[] = {indices->m_GraphicsFamily.value(), indices->m_PresentFamily.value()};

    VkSwapchainCreateInfoKHR SC_createInfo = {};

    SC_createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SC_createInfo.surface = m_Surface;
    SC_createInfo.minImageCount = imageCount;
    SC_createInfo.imageFormat = surfaceFormat.format;
    SC_createInfo.imageColorSpace = surfaceFormat.colorSpace;
    SC_createInfo.imageExtent = extent;
    SC_createInfo.imageArrayLayers = 1;
    SC_createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (indices->m_GraphicsFamily != indices->m_PresentFamily)
    {
        SC_createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        SC_createInfo.queueFamilyIndexCount = 2;
        SC_createInfo.pQueueFamilyIndices = QueueFamilyIndices;
    }
    else
    {
        SC_createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    SC_createInfo.preTransform = SC_Support_Details->m_Capabilities.currentTransform;
    SC_createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SC_createInfo.presentMode = presentMode;
    SC_createInfo.clipped = true;
    SC_createInfo.oldSwapchain = nullptr;

    // Extent, surfaceFormat, imageCount
    return std::pair<VkResult, uint32_t>(vkCreateSwapchainKHR(m_Device, &SC_createInfo, nullptr, &m_SwapChain), imageCount);
}

// Fonte:
// https://vulkan-tutorial.com/code/02_validation_layers.cpp

std::vector<const char *> V::get_required_extensions()
{
    uint32_t ENABLED_REQUIRED_EXTENSIONS_COUNT = 0;

    const char *const *instance_extensions = nullptr;
    instance_extensions = SDL_Vulkan_GetInstanceExtensions(&ENABLED_REQUIRED_EXTENSIONS_COUNT);

    if (instance_extensions == nullptr)
    {
        std::cerr << "[VULKAN ERROR]: Failed to get required extensions.\n";
    }

    std::vector<const char *> extensions(instance_extensions, instance_extensions + ENABLED_REQUIRED_EXTENSIONS_COUNT);

    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

bool V::checkValidationLayerSupport()
{
    uint32_t layerCount;

    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : m_ValidationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

VkResult V::CreateInstance(bool isValidationOn, std::string app_name, VkDebugUtilsMessengerCreateInfoEXT *DebugUtilsMessengerCreateInfoEXT)
{
    VkApplicationInfo AppInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = app_name.c_str(),
        .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .pEngineName = "NXLL",
        .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .apiVersion = VK_API_VERSION_1_2,
    };

    auto instance_extensions = get_required_extensions();
    const char *const *ppEnabledLayerNames = nullptr;
    void *pNext = nullptr;
    uint32_t EnabledLayerCount = 0;

    if (isValidationOn)
    {

        EnabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
        ppEnabledLayerNames = m_ValidationLayers.data();
        pNext = (VkDebugUtilsMessengerCreateInfoEXT *)DebugUtilsMessengerCreateInfoEXT;
    }

    VkInstanceCreateInfo CreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = pNext,
        .flags = {},
        .pApplicationInfo = &AppInfo,
        .enabledLayerCount = EnabledLayerCount,
        .ppEnabledLayerNames = ppEnabledLayerNames,
        .enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size()),
        .ppEnabledExtensionNames = instance_extensions.data(),
    };

    return vkCreateInstance(&CreateInfo, nullptr, &m_Vulkan_Instance);
}

std::string V::ErrorOfPhysicalDeviceCreation(const std::vector<const char *> deviceExtensions,
                                             VkSurfaceKHR surface)
{
    // Fail Check
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Vulkan_Instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        return std::string("[VULKAN ERROR]: Failed to find any gpu with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Vulkan_Instance, &deviceCount, devices.data());

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    int i = 0;
    for (const auto &device : devices)
    {
        // First check
        uint32_t ExtensionCount = 0;
        SDL_assert(!vkEnumerateDeviceExtensionProperties(device, nullptr, &ExtensionCount, nullptr) && "Failed to initialize ExtensionCount (PhysicalDevice).\n");
        std::vector<VkExtensionProperties> AvailableExtensionsProperties(ExtensionCount);
        SDL_assert(!vkEnumerateDeviceExtensionProperties(device, nullptr, &ExtensionCount, AvailableExtensionsProperties.data()) &&
                   "Failed to initialize AvailableExtensionsProperties.data() (PhysicalDevice)\n.");

        std::set<std::string> RequiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        for (const auto extension : AvailableExtensionsProperties)
        {
            RequiredExtensions.erase(extension.extensionName); // Check if it's empty
        }

        // If the Required extensions is empty, means that the user's gpu support all required extensions.
        if (!RequiredExtensions.empty())
            continue;

        // Second check check
        QueueFamilyIndices indices2 = QueueFamilyIndices::FindQueueFamilies(device, surface);
        if (!indices2.HasValue())
            continue;

        SwapChainSupportDetails supportDetails(device, surface);
        // Se alguma destas duas estiver vazias, azedou o pé do frango.
        if (supportDetails.m_Formats.empty() || supportDetails.m_PresentModes.empty())
            continue;

        // Third check check check
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            deviceFeatures.geometryShader)
        {
            m_Physical_Device = device;

            break;
        }

        i++;
    }

    if (&m_Physical_Device == nullptr)
    { // this shieet
        return std::string("[VULKAN ERROR]: Failed to find suitable GPU that supports Vulkan to use NXLL.");
    }

    return "";
}
// https://vulkan-tutorial.com
// "An image view is quite literally a view into an image. It describes how to
// access the image and which part of the image to access, for example if it
// should be treated as a 2D texture depth texture without any mipmapping levels.

// In this chapter we'll write a createImageViews function that creates a basic
// image view for every image in the swap chain so that we can use them as color targets later on.""

VkResult V::CreateimageViews()
{
    m_SwapChain_Image_Views.resize(m_SwapChain_Images.size());

    for (size_t i = 0; i < m_SwapChain_Images.size(); i++)
    {
        VkComponentMapping components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        };

        VkImageSubresourceRange imageSubResourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        VkImageViewCreateInfo IVcreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_SwapChain_Images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = m_SwapChain_Image_Format,
            .components = components,
            .subresourceRange = imageSubResourceRange,

        };

        return vkCreateImageView(m_Device, &IVcreateInfo, nullptr, &m_SwapChain_Image_Views[i]);
    }
}

V::V(std::string app_name, bool isValidationOn, SDL_Window *window)
{
    VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfoEXT = {};

    if (isValidationOn)
    {
        if (!checkValidationLayerSupport())
        {
            std::cerr << "[VULKAN ERROR]: Validation layers requested, but not available!\n";
            exit(1);
        }

        DebugUtilsMessengerCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugUtilsMessengerCreateInfoEXT.pNext = nullptr;
        DebugUtilsMessengerCreateInfoEXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugUtilsMessengerCreateInfoEXT.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugUtilsMessengerCreateInfoEXT.pfnUserCallback = &debugCallback;
    }

    if (CreateInstance(isValidationOn, app_name, &DebugUtilsMessengerCreateInfoEXT) != VK_SUCCESS)
    {
        std::cerr << "[VULKAN ERROR]: Failed to create vulkan instance.\n";
        exit(1);
    }

    std::cout << "[VULKAN INFO]: Succeded to create vulkan instance called Josh.\n";

    if (isValidationOn)
    {
        if (CreateDebugUtilsMessengerEXT(m_Vulkan_Instance, &DebugUtilsMessengerCreateInfoEXT, nullptr, &m_DebugMessenger) != VK_SUCCESS)
        {
            std::cerr << "[VULKAN ERROR]: Failed to set up debug messenger!\n";
            exit(1);
        }
        std::cout << "[VULKAN INFO]: Created debug messenger!\n";

        if (&m_DebugMessenger == nullptr)
        {
            std::cout << "[VULKAN INFO]: m_DebugMessenger still is nullptr!\n";
        }

        // Fonte:
        // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance
        // uint32_t extensionCount = 0;

        // vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        // std::vector<VkExtensionProperties> extensions(extensionCount);

        // Query the extension details:
        // vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        // Each VkExtensionProperties struct contains the name and version of an extension.
        // std::cout << "Available extensions:\n";
        // for (const auto &extension : extensions)
        //{
        //    std::cout << '\t' << extension.extensionName << '\n';
        //}
    }

    if (!SDL_Vulkan_CreateSurface(window, m_Vulkan_Instance, nullptr, &m_Surface))
    {
        std::cerr << "[VULKAN ERROR]: Failed to create vulkan rendering surface to the window.\n"
                  << SDL_GetError() << "\n";

        exit(1);
    }
    std::println("[VULKAN INFO]: Successfully create vulkan rendering surface to the window.");

    // "Pick" the Physical Device.
    std::string error = ErrorOfPhysicalDeviceCreation(m_DeviceExtensions,
                                                      m_Surface);
    if (error != "")
    {
        std::cerr << error << "\n";
        exit(1);
    }

    std::println("[VULKAN INFO]: Created physical device successfully.");

    QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(m_Physical_Device, m_Surface);
    SwapChainSupportDetails SC_SupportDetails(m_Physical_Device, m_Surface);

    if (CreateLogicalDevice(isValidationOn, &indices) != VK_SUCCESS)
    {
        std::cerr << "[VULKAN ERROR]: Failed to create logical device.\n";
        exit(1);
    }
    std::println("[VULKAN INFO]: Created logical device successfully.");

    vkGetDeviceQueue(m_Device, indices.m_GraphicsFamily.value(), 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, indices.m_PresentFamily.value(), 0, &m_PresentQueue);

    // Criação da swapChain.
    int32_t width, height;
    if (!SDL_GetWindowSize(window, &width, &height))
    {
        std::cerr << "[VULKAN ERROR]: SDL_GetWindowSize error:\n"
                  << SDL_GetError() << "\n";
        exit(1);
    }

    SDL_assert(width && height && "Width and height are null.\n");

    // Select the SwapChain format.
    VkSurfaceFormatKHR surfaceFormat = SC_SupportDetails.ChooseSurfaceCapabilities();

    VkExtent2D extent = SC_SupportDetails.ChooseExtent2D(width, height);

    // Result of SwapChain creation and the image count (refresh rate?) variable.
    std::pair<VkResult, uint32_t> ResultPair = CreateSwapChain(&SC_SupportDetails, surfaceFormat, extent, &indices);
    if (ResultPair.first != VK_SUCCESS)
    {
        std::cerr << "[VULKAN ERROR]: Failed to create SwapChain.\n";
        exit(1);
    }

    vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &ResultPair.second, nullptr);
    m_SwapChain_Images.resize(ResultPair.second);

    vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &ResultPair.second, m_SwapChain_Images.data());

    m_SwapChain_Image_Format = surfaceFormat.format;
    m_SwapChain_Extent = extent;

    std::println("[VULKAN INFO]: Succeded to create SwapChain.");

    if (CreateimageViews() != VK_SUCCESS)
    {
        std::cerr << "Failed to create image views.\n";
        exit(1);
    }
    std::println("[VULKAN INFO]: Succeded to create Image Views.");

    
};

V::~V()
{
    for (auto imageView : m_SwapChain_Image_Views)
    {
        vkDestroyImageView(m_Device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
    SDL_Vulkan_DestroySurface(m_Vulkan_Instance, m_Surface, nullptr);
    // vkDestroySurfaceKHR(m_Vulkan_Instance, m_Surface, nullptr);
    vkDestroyDevice(m_Device, nullptr);
    vkDestroyInstance(m_Vulkan_Instance, nullptr);

    std::println("[VULKAN INFO]: Succesfully destroyed all vulkan objects.");
}

/*
    VK_KHR_device_group_creation
    VK_KHR_external_fence_capabilities
    VK_KHR_external_memory_capabilities
    VK_KHR_external_semaphore_capabilities
    VK_KHR_get_physical_device_properties2
    VK_KHR_get_surface_capabilities2
    VK_KHR_surface
    VK_KHR_win32_surface
    VK_EXT_debug_report
    VK_EXT_debug_utils
    VK_EXT_swapchain_colorspace
    VK_KHR_portability_enumeration
    VK_LUNARG_direct_driver_loading
*/

// std::string V::HasVulkanSupport()
//{
// };
