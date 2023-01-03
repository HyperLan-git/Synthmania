#include "Gamemode.hpp"
Gamemode::Gamemode() {}

void Gamemode::onSpawn(Gui *g) {}
void Gamemode::onClockAdjust(int64_t value) {}
void Gamemode::onConfigChange() {}

size_t Gamemode::updateUBO(void *&ubo) { return sizeof(UniformBufferObject); }
void Gamemode::freeUBO(void *&ubo) {}

size_t Gamemode::updateFinalUBO(void *&ubo) {
    return sizeof(UniformBufferObject);
}
void Gamemode::freeFinalUBO(void *&ubo) {}

Gamemode::~Gamemode() {}
