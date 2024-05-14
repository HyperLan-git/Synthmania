#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <map>
#include <set>
#include <stdexcept>
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

// TODO put as many vulkan calls as possible in this class
class Device {
   public:
    Device(VkPhysicalDevice &physicalDevice,
           const std::vector<const char *> deviceExtensions,
           std::map<std::string, FamilyPredicate> familyPredicates,
           const std::vector<const char *> validationLayers =
               std::vector<const char *>());

    Device(Device &&);
    Device &operator=(Device &&);

    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;

    bool operator==(const Device &);

    VkDevice getDevice();
    VkPhysicalDevice getPhysicalDevice();
    std::optional<Queue> getQueue(std::string name);
    void wait();
    ~Device();

   private:
    VkPhysicalDevice &physicalDevice;
    VkDevice device;
    std::map<std::string, FamilyData> queues;
};