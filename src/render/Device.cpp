#include "Device.hpp"

#include <iostream>

std::vector<uint32_t> findQueueFamilies(
    VkPhysicalDevice device, std::vector<FamilyPredicate> familyPredicates)
{
    std::vector<uint32_t> result = {};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             queueFamilies.data());

    uint32_t j = 0;
    uint32_t i = 0;
    for (i = 0; i < queueFamilies.size(); i++)
    {
        const VkQueueFamilyProperties &queueFamily = queueFamilies[i];
        if (familyPredicates[j](device, queueFamily, i))
        {
            result.push_back(i);
            j++;
            if (j >= familyPredicates.size())
                return result;
            i = -1;
        }
    }
    if (i >= queueFamilies.size() || result.size() < familyPredicates.size())
        result.clear();

    return result;
}

Device::Device(VkPhysicalDevice *physicalDevice,
               const std::vector<const char *> deviceExtensions,
               std::vector<FamilyPredicate> familyPredicates,
               const std::vector<const char *> validationLayers) {
    this->queues = {};
    this->device = new VkDevice();
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::vector<uint32_t> queueFamilies =
        findQueueFamilies(*physicalDevice, familyPredicates);
    std::cout << "familyCount:" << queueFamilies.size() << std::endl;
    std::set<uint32_t> uniqueQueueFamilies = {};

    float queuePriority = 1.0f;
    for (int i = 0; i < queueFamilies.size(); i++)
    {
        auto result = uniqueQueueFamilies.emplace(queueFamilies[i]);
        if (result.second)
        {
            int j = 0;
            for (auto it = uniqueQueueFamilies.begin(); *it != *(result.first);
                 it++)
                j++;
            queuesID[i] = j;
        }
    }

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.geometryShader = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    if (vkCreateDevice(*physicalDevice, &createInfo, nullptr, device) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    int i = 0;
    for (uint32_t family : uniqueQueueFamilies)
    {
        std::cout << "family:" << family << std::endl;
        queues.push_back(new Queue(this, family, 0));
    }
}

VkDevice *Device::getDevice() { return device; }

Queue *Device::getQueue(int id)
{
    if (id >= queuesID.size())
        id = queuesID.size() - 1;
    return this->queues[queuesID[id]];
}

void Device::wait() { vkDeviceWaitIdle(*device); }

Device::~Device()
{
    vkDestroyDevice(*device, nullptr);
    for (Queue *queue : queues)
    {
        delete queue;
    }
    delete device;
}