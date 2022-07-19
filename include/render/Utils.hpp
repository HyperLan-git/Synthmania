#pragma once

#include <vulkan/vulkan.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Gui.hpp"
#include "ImageView.hpp"
#include "Window.hpp"

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

uint32_t findMemoryType(VkPhysicalDevice* physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);
std::vector<char> readFile(const std::string& filename);

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice,
                             const std::vector<VkFormat>& candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);

SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);
VkSurfaceFormatKHR chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats);

VkPresentModeKHR chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes);

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                            Window* window);

ImageView* getTextureByName(std::vector<ImageView*> textures, const char* name);

struct DebugFunc {
    PFN_vkSetDebugUtilsObjectTagEXT setTag;
    PFN_vkSetDebugUtilsObjectNameEXT setName;
    PFN_vkCmdBeginDebugUtilsLabelEXT begin;
    PFN_vkCmdEndDebugUtilsLabelEXT end;
    PFN_vkCmdInsertDebugUtilsLabelEXT insert;
};

/**
 * @brief Gets functions to debug using object names, returns a struct full of
 * NULL pointers if NDEBUG is set
 *
 * @param device a valid device with the right extension
 * @return DebugFunc useful debug functions that may be used for naming vulkan
 * objects, for instance
 */
DebugFunc getDebugFunctions(Device* device);

/**
 * @brief Set a vulkan object's name for debugging purposes, will do nothing if
 * NDEBUG is set
 *
 * @param debugFunctions debug functions as returned by getDebugFunctions
 * @param device the logical device
 * @param name the name to assign
 * @param type the object's type
 * @param obj the pointer to the vulkan object
 */
void setName(DebugFunc debugFunctions, Device* device, std::string name,
             VkObjectType type, void* obj);

/**
 * @brief Begins a new section in a command buffer, will do nothing if NDEBUG is
 * set
 *
 * @param debugFunctions debug functions as returned by getDebugFunctions
 * @param name name of the section
 * @param buffer command buffer which contains the section
 */
void beginSection(DebugFunc debugFunctions, std::string name,
                  CommandBuffer* buffer);

/**
 * @brief Ends a debug section previously created by beginSection, will do
 * nothing if NDEBUG is set
 *
 *
 * @param debugFunctions debug functions as returned by getDebugFunctions
 * @param buffer command buffer which contains the section
 */
void endSection(DebugFunc debugFunctions, CommandBuffer* buffer);

typedef int (*GuiOrderFunction)(Gui*, Gui*);

void sortGuis(std::vector<Gui*>& guis, GuiOrderFunction comparator);

int cmpGuisByTexture(Gui* gui, Gui* gui2);