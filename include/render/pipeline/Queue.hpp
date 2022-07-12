#pragma once

class Queue;

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "Device.hpp"

class Queue {
   public:
    Queue(Device *device, uint32_t family, uint32_t id, std::string name);
    VkQueue *getQueue();
    uint32_t getFamily();
    uint32_t getID();
    std::string getName();
    ~Queue();

   private:
    Device *device;
    std::string name;
    uint32_t queue_id;
    uint32_t family_id;
    VkQueue *queue;
};