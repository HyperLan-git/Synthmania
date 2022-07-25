#include "Device.hpp"

#include <iostream>

void getQueueFamilies(VkPhysicalDevice physicalDevice,
                      std::vector<VkQueueFamilyProperties> &queueFamilies) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             nullptr);

    queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             queueFamilies.data());
}

std::map<std::string, FamilyData> findQueueFamilies(
    VkPhysicalDevice device, std::vector<VkQueueFamilyProperties> queueFamilies,
    std::map<std::string, FamilyPredicate> familyPredicates) {
    std::map<std::string, FamilyData> result;

    uint32_t i = 0;
    for (auto entry : familyPredicates)
        for (i = 0; i < queueFamilies.size(); i++) {
            VkQueueFamilyProperties *queueFamily =
                new VkQueueFamilyProperties(queueFamilies[i]);
            if (queueFamily->queueCount > 0 &&
                entry.second(device, *queueFamily, i)) {
                result.emplace(
                    entry.first,
                    FamilyData({.id = i, .properties = queueFamily}));
                break;
            }
            delete queueFamily;
        }

    return result;
}

Device::Device(VkPhysicalDevice *physicalDevice,
               const std::vector<const char *> deviceExtensions,
               std::map<std::string, FamilyPredicate> familyPredicates,
               const std::vector<const char *> validationLayers) {
    this->queues = {};
    this->device = new VkDevice();
    std::vector<VkQueueFamilyProperties> families;
    getQueueFamilies(*physicalDevice, families);
    std::map<std::string, FamilyData> queueFamilies =
        findQueueFamilies(*physicalDevice, families, familyPredicates);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::vector<float *> tabs;
    std::vector<FamilyData> createdFamilies;
    for (auto &queueFamily : queueFamilies) {
        bool c = false;
        for (FamilyData &data : createdFamilies)
            if (data.id == queueFamily.second.id) {
                queues.emplace(queueFamily.first, data);
                c = true;
                break;
            }
        if (!c && queues.find(queueFamily.first) == queues.end()) {
            FamilyData &data = queueFamily.second;
            uint32_t count = data.properties->queueCount;
            float *queuePriorities = new float[count];
            tabs.push_back(queuePriorities);
            for (int i = 0; i < count; i++) queuePriorities[i] = 1.f;
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily.second.id;
            queueCreateInfo.queueCount = count;
            queueCreateInfo.pQueuePriorities = queuePriorities;
            queueCreateInfos.emplace_back(queueCreateInfo);
            queues.emplace(queueFamily.first, queueFamily.second);
            createdFamilies.push_back(queueFamily.second);
        } else
            delete queueFamily.second.properties;
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.geometryShader = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;

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

    if (vkCreateDevice(*physicalDevice, &createInfo, NULL, device) !=
        VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");

    for (float *p : tabs) delete[] p;

    for (auto &entry : queues)
        for (int i = 0; i < entry.second.properties->queueCount; i++)
            entry.second.queues.push_back(
                new Queue(this, entry.second.id, i, entry.first));
}

VkDevice *Device::getDevice() { return device; }

Queue *Device::getQueue(std::string name) {
    for (auto entry : queues) {
        if (entry.first.compare(name.c_str()) == 0)
            return Queue::getAvailableQueue(entry.second.queues);
    }
    return NULL;
}

void Device::wait() { vkDeviceWaitIdle(*device); }

Device::~Device() {
    vkDestroyDevice(*device, nullptr);
    std::set<VkQueueFamilyProperties *> got;
    for (auto entry : queues) {
        if (got.find(entry.second.properties) == got.end()) {
            delete entry.second.properties;
            got.emplace(entry.second.properties);
        }
        for (Queue *queue : entry.second.queues) delete queue;
    }
    delete device;
}