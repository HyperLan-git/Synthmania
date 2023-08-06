#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <map>
#include <set>
#include <string>

class Device;

typedef std::function<bool(VkPhysicalDevice, VkQueueFamilyProperties, uint32_t)>
    FamilyPredicate;

#include "Queue.hpp"

void getQueueFamilies(VkPhysicalDevice physicalDevice,
                      std::vector<VkQueueFamilyProperties> &queueFamilies);

std::map<std::string, FamilyData> findQueueFamilies(
    VkPhysicalDevice device, std::vector<VkQueueFamilyProperties> queueFamilies,
    std::map<std::string, FamilyPredicate> familyPredicates);

class Device {
   public:
    Device(VkPhysicalDevice *physicalDevice,
           const std::vector<const char *> deviceExtensions,
           std::map<std::string, FamilyPredicate> familyPredicates,
           const std::vector<const char *> validationLayers =
               std::vector<const char *>());
    VkDevice getDevice();
    Queue *getQueue(std::string name);
    void wait();
    ~Device();

   private:
    VkDevice device;
    std::map<std::string, FamilyData> queues;
};