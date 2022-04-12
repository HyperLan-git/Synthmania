#include "Queue.hpp"

Queue::Queue(Device *device, uint32_t family, uint32_t id) {
    this->device = device;
    this->queue = new VkQueue();
    vkGetDeviceQueue(*(device->getDevice()), family, id, queue);
}

VkQueue *Queue::getQueue() { return queue; }

uint32_t Queue::getFamily() { return family_id; }

uint32_t Queue::getID() { return queue_id; }

Queue::~Queue() { delete queue; }