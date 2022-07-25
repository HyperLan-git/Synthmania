#pragma once

class GraphicalEffectHandler;

#include "GraphicalEffect.hpp"
#include "Synthmania.hpp"

/**
 * @brief Returns your Graphics handler for this mod
 */
extern "C" GraphicalEffectHandler* getEffectHandler(Synthmania* game);

/**
 * @brief A class powerful enough (hopefully) to make modcharts to rival NotITG
 * itself. Feel free to modify or replace variables in the game, as long as you
 * know what you are doing everything will be fine
 */
class GraphicalEffectHandler {
   public:
    GraphicalEffectHandler(Synthmania* game);

    /**
     * @brief Gets called whenever a gui appears
     *
     * @param g The gui about to appear
     */
    virtual void onSpawn(Gui* g) = 0;

    /**
     * @brief Gets the vertex shader code (if you don't know how to write
     * glsl then learn right now or else)
     *
     * @return A string containing path to a valid spv file else an empty string
     * if you don't know what you're doing (just like me !)
     */
    virtual const std::string getVertShaderCode() = 0;

    /**
     * @brief Gets the fragment shader code (if you don't know how to write
     * glsl then learn right now or else)
     *
     * @return A string containing path to a valid spv file else an empty string
     * if you don't know what you're doing (just like me !)
     */
    virtual const std::string getFragShaderCode() = 0;

    /**
     * @brief This function gets called every frame after elements ingame are
     * updated
     *
     * @param time The current time in microseconds (I never do stuff that's
     * overkill don't worry)
     */
    virtual void update(int64_t time) = 0;

    /**
     * @brief This should return sizeof(UniformBufferObject) unless you are
     * changing the shader
     *
     * @return maximum size you may return for updateUBO(void*&, int64_t)
     */
    virtual size_t getUBOSize() = 0;

    /**
     * @brief Updates UBO for the current frame, you can replace the pointer
     * without causing errors or memory leaks
     *
     * @param ubo The pointer to uniforms
     * @param time Current time in micros
     * @return the size of the data written
     * (should be sizeof(UniformBufferObject) by default)
     * CANNOT be higher than what you return in getUBOSize()
     * else you will cause a SegFault
     */
    virtual size_t updateUBO(void*& ubo, int64_t time) = 0;

    /**
     * @brief Do nothing if you did not replace ubo, else free or delete the
     * pointer in order to avoid memory leaks
     *
     * @param ubo The pointer as you left it at the end of updateUBO(void*&,
     * int64_t)
     */
    virtual void freeUBO(void*& ubo) = 0;

    virtual ~GraphicalEffectHandler() = default;

   protected:
    Synthmania* game;
};