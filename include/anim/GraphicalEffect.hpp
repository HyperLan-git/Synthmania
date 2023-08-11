#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <random>

class GraphicalEffect;

#include "Gui.hpp"

typedef glm::vec2 (*GraphicalEffectFunction)(int64_t time, Gui& gui,
                                             std::vector<float>& data,
                                             std::vector<int64_t>& iData);

class GraphicalEffect {
   public:
    GraphicalEffect(GraphicalEffectFunction function,
                    std::initializer_list<float> data = {},
                    std::initializer_list<int64_t> tData = {});

    virtual glm::vec2 operator()(Gui& gui, int64_t time);

    virtual ~GraphicalEffect();

   protected:
    GraphicalEffectFunction function;
    std::vector<float> data;
    std::vector<int64_t> tData;
};

glm::vec2 applyShaking(int64_t time, Gui& gui, std::vector<float>& data,
                       std::vector<int64_t>& iData);

glm::vec2 applyTemp(int64_t time, Gui& gui, std::vector<float>& data,
                    std::vector<int64_t>& iData);

glm::vec2 applyOffset(int64_t time, Gui& gui, std::vector<float>& data,
                      std::vector<int64_t>& iData);

glm::vec2 applyWavy(int64_t time, Gui& gui, std::vector<float>& data,
                    std::vector<int64_t>& tData);