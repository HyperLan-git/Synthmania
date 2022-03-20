#pragma once

class Device;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>

typedef std::function<bool(VkPhysicalDevice, VkQueueFamilyProperties, uint32_t)>
    FamilyPredicate;

#include "Queue.hpp"

std::vector<uint32_t> findQueueFamilies(
    VkPhysicalDevice device, std::vector<FamilyPredicate> familyPredicates);

class Device {
   public:
    Device(VkPhysicalDevice *physicalDevice,
           const std::vector<const char *> deviceExtensions,
           std::vector<FamilyPredicate> familyPredicates);
    Device(VkPhysicalDevice *physicalDevice,
           const std::vector<const char *> deviceExtensions,
           std::vector<FamilyPredicate> familyPredicates,
           const std::vector<const char *> validationLayers);
    VkDevice *getDevice();
    Queue *getQueue(int queueID);
    void wait();
    ~Device();

   private:
    VkDevice *device;
    std::vector<Queue *> queues;
};