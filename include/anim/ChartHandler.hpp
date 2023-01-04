#pragma once

class ChartHandler;

#include "GraphicalEffect.hpp"
#include "Synthmania.hpp"

/**
 * @brief Returns your Graphics handler for this mod
 */
extern "C" ChartHandler* getChartHandler(Synthmania* game);

/**
 * @brief A class powerful enough (hopefully) to make modcharts or cool
 * animations. Feel free to modify or replace variables in the game, as long as
 * you know what you are doing everything will be fine
 */
class ChartHandler {
   public:
    ChartHandler(Synthmania* game);

    /**
     * @brief Gets called whenever a gui appears
     *
     * @param g The gui about to appear
     */
    virtual void onSpawn(std::shared_ptr<Gui>& g) = 0;

    /**
     * @brief Gets the vertex shader code (if you don't know how to write
     * glsl then learn right now or else)
     *
     * @return A string containing path to a valid spv file else an empty string
     * if you don't know what you're doing (just like me !)
     */
    virtual const std::string getVertShaderCode() = 0;

    /**
     * @brief Gets the geometry shader code (if you don't know how to write
     * glsl then learn right now or else)
     *
     * @return A string containing path to a valid spv file else an empty string
     * if you don't know what you're doing (just like me !)
     */
    virtual const std::string getGeomShaderCode() = 0;

    /**
     * @brief Gets the fragment shader code (if you don't know how to write
     * glsl then learn right now or else)
     *
     * @return A string containing path to a valid spv file else an empty string
     * if you don't know what you're doing (just like me !)
     */
    virtual const std::string getFragShaderCode() = 0;

    /**
     * @brief Gets the final pass vertex shader code
     * the final pass is when the entire screen has been rendered to a texture
     * meaning you can transform that
     *
     * @return A string containing path to a valid spv file else an empty string
     * if you don't know what you're doing (just like me !)
     */
    virtual const std::string getFinalVertShaderCode() = 0;

    /**
     * @brief Gets the final pass geometry shader code
     * the final pass is when the entire screen has been rendered to a texture
     * meaning you can transform that
     *
     * @return A string containing path to a valid spv file else an empty string
     * if you don't know what you're doing (just like me !)
     */
    virtual const std::string getFinalGeomShaderCode() = 0;

    /**
     * @brief Gets the final pass fragment shader code
     * the final pass is when the entire screen has been rendered to a texture
     * meaning you can transform that
     *
     * @return A string containing path to a valid spv file else an empty string
     * if you don't know what you're doing (just like me !)
     */
    virtual const std::string getFinalFragShaderCode() = 0;

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
     * changing the shaders
     *
     * @return maximum size you may return for updateUBO(void*&, int64_t)
     */
    virtual size_t getUBOSize() = 0;

    /**
     * @brief This should return sizeof(UniformBufferObject) unless you are
     * changing the final shaders
     *
     * @return maximum size you may return for updateFinalUBO(void*&, int64_t)
     */
    virtual size_t getFinalUBOSize() = 0;

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
     * @brief Updates UBO used in the shaders of the second pass
     *
     * @param ubo The pointer to uniforms
     * @param time Current time in micros
     * @return the size of the data written
     * (should be sizeof(UniformBufferObject) by default)
     * CANNOT be higher than what you return in getFinalUBOSize()
     * else you will cause a SegFault
     */
    virtual size_t updateFinalUBO(void*& ubo, int64_t time) = 0;

    /**
     * @brief Do nothing if you did not replace ubo in updateUBO, else free or
     * delete the pointer in order to avoid memory leaks
     *
     * @param ubo The pointer as you left it at the end of updateUBO(void*&,
     * int64_t)
     */
    virtual void freeUBO(void*& ubo) = 0;

    /**
     * @brief Do nothing if you did not replace ubo in updateFinalUBO, else free
     * or delete the pointer in order to avoid memory leaks
     *
     * @param ubo The pointer as you left it at the end of
     * updateFinalUBO(void*&, int64_t)
     */
    virtual void freeFinalUBO(void*& ubo) = 0;

    virtual ~ChartHandler() = default;

   protected:
    Synthmania* game;
};