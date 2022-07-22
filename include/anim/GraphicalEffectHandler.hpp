#pragma once

class GraphicalEffectHandler;

#include "GraphicalEffect.hpp"
#include "Synthmania.hpp"

extern "C" GraphicalEffectHandler* getEffectHandler(Synthmania* game);

class GraphicalEffectHandler {
   public:
    GraphicalEffectHandler(Synthmania* game);

    virtual void onSpawn(Gui* g) = 0;

    virtual void update(int64_t time) = 0;

    /**
     * @brief Updates UBO for the current frame
     *
     * @param ubo The pointer to uniforms
     * @param time Current time in micros
     * @return int the size of the struct
     * (should be sizeof(UniformBufferObject) by default)
     */
    virtual size_t updateUBO(void*& ubo, int64_t time) = 0;

    virtual ~GraphicalEffectHandler() = default;

   protected:
    Synthmania* game;
};