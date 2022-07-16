#include "Queue.hpp"

Queue::Queue(Device *device, uint32_t family, uint32_t id, std::string name) {
    this->device = device;
    this->name = name;
    this->family_id = family;
    this->queue_id = id;
    this->queue = new VkQueue();
    vkGetDeviceQueue(*(device->getDevice()), family, id, queue);
}

VkQueue *Queue::getQueue() { return queue; }

uint32_t Queue::getFamily() { return family_id; }

uint32_t Queue::getID() { return queue_id; }

std::string Queue::getName() { return name; }

Queue *Queue::getAvailableQueue(std::vector<Queue *> queues) {
    if (queues.empty()) return NULL;
    return queues[0];
}

Queue::~Queue() { delete queue; }