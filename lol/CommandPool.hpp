#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <stdexcept>
#include <vector>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                     VkSurfaceKHR surface);

class CommandPool {
   public:
    CommandPool(VkPhysicalDevice physicalDevice, VkDevice* device,
                VkSurfaceKHR toSupport);
    VkCommandPool* getPool();
    ~CommandPool();

   private:
    VkDevice* device;
    VkCommandPool* pool;
};