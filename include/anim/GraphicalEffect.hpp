#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <random>

class GraphicalEffect;

#include "Gui.hpp"

typedef glm::vec2 (*GraphicalEffectFunction)(int64_t time, Gui* gui,
                                             float*& data, int64_t*& iData);

class GraphicalEffect {
   public:
    GraphicalEffect(GraphicalEffectFunction function, float* data = NULL,
                    int64_t* tData = NULL);

    virtual glm::vec2 call(Gui* gui, int64_t time);

    virtual ~GraphicalEffect();

   protected:
    GraphicalEffectFunction function;
    float* data;
    int64_t* tData;
};

glm::vec2 applyShaking(int64_t time, Gui* gui, float*& data, int64_t*& iData);

glm::vec2 applyTemp(int64_t time, Gui* gui, float*& data, int64_t*& iData);

glm::vec2 applyOffset(int64_t time, Gui* gui, float*& data, int64_t*& iData);