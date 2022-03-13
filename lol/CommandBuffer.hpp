#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

class CommandBuffer {
   public:
    CommandBuffer(VkDevice *device, VkCommandPool commandPool);

    void reset();

    VkCommandBuffer *getBuffer();

    ~CommandBuffer();

   private:
    VkCommandBuffer *buffer;
    VkDevice *device;
    VkCommandPool pool;
};