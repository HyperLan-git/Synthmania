#pragma once

class Queue;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "Device.hpp"

class Queue {
   public:
    Queue(Device *device, uint32_t family, uint32_t id);
    VkQueue *getQueue();
    uint32_t getFamily();
    uint32_t getID();
    ~Queue();

   private:
    Device *device;
    uint32_t queue_id;
    uint32_t family_id;
    VkQueue *queue;
};