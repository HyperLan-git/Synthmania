#pragma once

class Device;

#include <vulkan/vulkan.h>

#include <functional>
#include <map>
#include <optional>
#include <set>
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
           std::vector<FamilyPredicate> familyPredicates,
           const std::vector<const char *> validationLayers =
               std::vector<const char *>());
    VkDevice *getDevice();
    Queue *getQueue(int id);
    void wait();
    ~Device();

private:
    VkDevice *device;
    std::vector<Queue *> queues;
    std::map<int, int> queuesID;
};