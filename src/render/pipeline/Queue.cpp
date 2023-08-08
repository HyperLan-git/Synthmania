#include "Queue.hpp"

Queue::Queue(Device &device, uint32_t family, uint32_t id, std::string name)
    : device(device) {
    this->name = name;
    this->family_id = family;
    this->queue_id = id;
    vkGetDeviceQueue(device.getDevice(), family, id, &queue);
}

Queue::Queue(Queue &&other) : device(other.device) { *this = other; }

Queue &Queue::operator=(Queue &&other) { return *this = other; }

Queue::Queue(const Queue &other)
    : device(other.device),
      queue(other.queue),
      family_id(other.family_id),
      queue_id(other.queue_id),
      name(other.name) {}

Queue &Queue::operator=(const Queue &other) {
    assert(this->device == other.device);
    this->queue = other.queue;
    this->family_id = other.family_id;
    this->queue_id = other.queue_id;
    this->name = other.name;
    return *this;
}

VkQueue Queue::getQueue() const { return queue; }

uint32_t Queue::getFamily() const { return family_id; }

uint32_t Queue::getID() const { return queue_id; }

std::string Queue::getName() const { return name; }

std::optional<Queue> Queue::getAvailableQueue(std::vector<Queue> &queues) {
    if (queues.empty()) return std::optional<Queue>();
    return std::make_optional(queues[0]);
}

VkResult Queue::wait() const { return vkQueueWaitIdle(queue); }

Queue::~Queue() {}