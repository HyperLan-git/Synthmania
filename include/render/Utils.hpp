#pragma once

#ifndef _WIN32
#include <dlfcn.h>
#else
#include <libloaderapi.h>
#endif
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

// TODO replace with map
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
DebugFunc getDebugFunctions(Instance* instance);

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
void setName(DebugFunc debugFunctions, Device* device, const std::string& name,
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

/**
 * @brief This function exists to preserve Z-ordering among guis
 * (it is necessary to handle transparency because I didn't think of anything
 * better)
 *
 * @param guis the vector to sort
 * @param comparator the ordering function
 */
void sortGuis(std::vector<Gui*>& guis, GuiOrderFunction comparator);

/**
 * @brief Compares guis' pointers to textures in order to sort them and get less
 * overhead while rendering
 *
 * @param gui First element to sort
 * @param gui2 Second element to sort
 * @return int texture1 - texture2
 */
int cmpGuisByTexture(Gui* gui, Gui* gui2);

/**
 * @brief This will return a pointer to a dynamic library handle, use
 * getFunction to get its contents
 *
 * @param file filename
 * @return void* Pointer to a dl handle
 */
void* loadShared(std::string file);

/**
 * @brief gets a pointer to a specified function in the library returned by
 * loadShared
 *
 * @param shared a handle returned by loadShared
 * @param fct the function name
 * @return void* a pointer to the function
 */
void* getFunction(void* shared, const char* fct);