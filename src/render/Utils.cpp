#include "Utils.hpp"

uint32_t findMemoryType(VkPhysicalDevice* physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(*physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice) {
    return findSupportedFormat(
        physicalDevice,
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
         VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice,
                             const std::vector<VkFormat>& candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                   (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface) {
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                              &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                             details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                              &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                            Window* window) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        uint32_t width, height;
        window->getFramebufferSize(&width, &height);

        VkExtent2D actualExtent = {width, height};

        actualExtent.width =
            std::clamp(width, capabilities.minImageExtent.width,
                       capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(height, capabilities.minImageExtent.height,
                       capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

ImageView* getTextureByName(std::vector<ImageView*> textures,
                            const char* name) {
    for (ImageView* img : textures)
        if (std::strcmp(name, img->getName().c_str()) == 0) return img;
    if (strcmp("missing", name) == 0) {
        return NULL;
    }
    return getTextureByName(textures, "missing");
}

DebugFunc getDebugFunctions(Device* device) {
    DebugFunc result;
#ifndef NDEBUG
    VkDevice d = *(device->getDevice());
    result.begin = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetDeviceProcAddr(
        d, "vkCmdBeginDebugUtilsLabelEXT");
    result.end = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetDeviceProcAddr(
        d, "vkCmdEndDebugUtilsLabelEXT");
    result.insert = (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetDeviceProcAddr(
        d, "vkCmdInsertDebugUtilsLabelEXT");
    result.setName = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(
        d, "vkSetDebugUtilsObjectNameEXT");
    result.setTag = (PFN_vkSetDebugUtilsObjectTagEXT)vkGetDeviceProcAddr(
        d, "vkSetDebugUtilsObjectTagEXT");
#else
    result.begin = NULL;
    result.end = NULL;
    result.insert = NULL;
    result.setName = NULL;
    result.setTag = NULL;
#endif

    return result;
}

void setName(DebugFunc debugFunctions, Device* device, std::string name,
             VkObjectType type, void* obj) {
#ifndef NDEBUG
    VkDebugUtilsObjectNameInfoEXT info;
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.objectHandle = (uint64_t)obj;
    info.objectType = type;
    info.pObjectName = name.c_str();
    info.pNext = NULL;
    debugFunctions.setName(*(device->getDevice()), &info);
#endif
}

void beginSection(DebugFunc debugFunctions, std::string name,
                  CommandBuffer* buffer) {
#ifndef NDEBUG
    VkDebugUtilsLabelEXT info;
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    info.pLabelName = name.c_str();
    info.color[0] = info.color[3] = 1.f;
    info.pNext = NULL;
    debugFunctions.begin(*(buffer->getBuffer()), &info);
#endif
}