#pragma once

#include <assert.h>

#include <optional>

// FIXME make a queuefamily class
class Queue;
struct FamilyData;

#include "Device.hpp"

struct FamilyData {
    uint32_t id;
    VkQueueFamilyProperties properties;
    std::vector<Queue> queues;
};

// XXX decide if we don't make a struct out of this?
class Queue {
   public:
    Queue(Device &device, uint32_t family, uint32_t id, std::string name);

    Queue(Queue &&);
    Queue &operator=(Queue &&);

    Queue(const Queue &);
    Queue &operator=(const Queue &);

    VkQueue getQueue() const;
    uint32_t getFamily() const;
    uint32_t getID() const;
    std::string getName() const;

    VkResult wait() const;

    ~Queue();

    static std::optional<Queue> getAvailableQueue(std::vector<Queue> &queues);

   private:
    Device &device;
    std::string name;
    uint32_t queue_id;
    uint32_t family_id;
    VkQueue queue;
};