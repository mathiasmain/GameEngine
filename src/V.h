// #pragma once
#include <vulkan/vulkan.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include <utility>
#include <set>
#include <print>
#include <expected>
#include <cassert>
#include <algorithm>

struct QueueFamilyIndices
{
public:
    std::optional<uint32_t> m_GraphicsFamily;
    std::optional<uint32_t> m_PresentFamily;

    bool HasValue()
    {
        return m_GraphicsFamily.has_value() && m_PresentFamily.has_value();
    }

    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices = {};
        uint32_t queueFamilyCount = 0;

        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        SDL_assert(queueFamilyCount != 0);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        SDL_assert(!queueFamilies.empty());

        uint32_t PresentSupport = 0;

        int i = 0;
        for (const auto &queueFamily : queueFamilies)
        {

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.m_GraphicsFamily = i;
            }

            PresentSupport = 0;
            // No exemplo original não há checagem se a função foi bem sucedida, portanto, se houver algum erro, olhe aqui primeiro:
            if (vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &PresentSupport) == VK_SUCCESS)
            {
                if (PresentSupport == 1)
                {
                    indices.m_PresentFamily = i;
                }
            }
            // Verif
            if (indices.HasValue())
                break;

            i++;

            if (!indices.HasValue())
            {
                std::cerr << "[VULKAN ERROR]: No GraphicsFamily was found.\n";
                exit(1);
            }
        }
        return indices;
    };
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR m_Capabilities;
    std::vector<VkSurfaceFormatKHR> m_Formats;
    std::vector<VkPresentModeKHR> m_PresentModes;

    SwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SDL_assert(!vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &this->m_Capabilities));

        uint32_t formatCount;
        SDL_assert(!vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));

        if (formatCount != 0)
        {
            this->m_Formats.resize(formatCount);
            SDL_assert(!vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, this->m_Formats.data()));
        }

        uint32_t presentModeCount;
        SDL_assert(!vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr));

        if (presentModeCount != 0)
        {
            this->m_PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, this->m_PresentModes.data());
        }
    };

    VkSurfaceFormatKHR ChooseSurfaceCapabilities()
    {
        for (const auto availableFormat : this->m_Formats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return this->m_Formats[0];
    }

    VkExtent2D ChooseExtent2D(int32_t width, int32_t height)
    {
        if (this->m_Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return this->m_Capabilities.currentExtent;
        }
        else
        {
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)};

            actualExtent.width = std::clamp(actualExtent.width, this->m_Capabilities.minImageExtent.width, this->m_Capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, this->m_Capabilities.minImageExtent.height, this->m_Capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    VkPresentModeKHR ChoosePresentMode()
    {
        for (const auto &availablePresentMode : this->m_PresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }
};

// Café, Frutas, Suco de Laranja (não serão mais expertados aos EUA).
// Produtos que apresentam vanjagem comercial nas exportações:
//      Petróleo Cru, Soja e alimentos derivados, Minério de ferro, Milho, Açucar bruto.

// Produto que o Brasil produz.
// Produto que o Brasil majoritariamente importa.
// Eletrônicos que o Brasil importa

class V
{
public:
    VkInstance m_Vulkan_Instance = nullptr;

    VkPhysicalDevice m_Physical_Device = nullptr;
    VkDevice m_Device = nullptr;

    VkQueue m_GraphicsQueue = nullptr;
    VkQueue m_PresentQueue = nullptr;

    VkSurfaceKHR m_Surface = nullptr;
    VkSwapchainKHR m_SwapChain = nullptr;
    std::vector<VkImage> m_SwapChain_Images;
    VkFormat m_SwapChain_Image_Format;
    VkExtent2D m_SwapChain_Extent;

    const std::vector<const char *> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    std::vector<const char *> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;

    std::vector<const char *> get_required_extensions();
    bool checkValidationLayerSupport();
    void cleanupDebugUtilsMessengerEXT();

    VkResult CreateInstance(bool isValidationOn, std::string app_name, VkDebugUtilsMessengerCreateInfoEXT *DebugUtilsMessengerCreateInfoEXT);
    // Todas estas funções servem para escolher o dispositivo mais próximo de
    // suportar todas as tecnologias requeridas e melhor performar.
    std::string ErrorOfPhysicalDeviceCreation(const std::vector<const char *> deviceExtensions,
                                              VkSurfaceKHR surface);

    VkResult CreateLogicalDevice(bool isValidationOn, QueueFamilyIndices *indices);
    std::pair<VkResult, uint32_t> CreateSwapChain(SwapChainSupportDetails *SC_Support_Details,
                                                  VkSurfaceFormatKHR surfaceFormat,
                                                  VkExtent2D extent, QueueFamilyIndices *indices);

    V(std::string app_name, bool isValidationOn, SDL_Window *window);
    ~V();
};

//// --------------------------------------------------------
// a
