#include "GraphicalEffect.hpp"

GraphicalEffect::GraphicalEffect(GraphicalEffectFunction function,
                                 std::initializer_list<float> data,
                                 std::initializer_list<int64_t> tData) {
    this->function = function;
    this->data = data;
    this->tData = tData;
}

glm::vec2 GraphicalEffect::operator()(Gui& gui, int64_t time) {
    return this->function(time, gui, data, tData);
}

GraphicalEffect::~GraphicalEffect() {}

glm::vec2 applyShaking(int64_t time, Gui& gui, std::vector<float>& data,
                       std::vector<int64_t>& tData) {
    if (data.empty()) data = {.1f};
    float amplitude = data[0];
    float r = std::rand() * amplitude / RAND_MAX - amplitude / 2.,
          r2 = std::rand() * amplitude / RAND_MAX - amplitude / 2.;
    return glm::vec2({r, r2});
}

glm::vec2 applyTemp(int64_t time, Gui& gui, std::vector<float>& data,
                    std::vector<int64_t>& tData) {
    if (data.size() < 2) data = {0.f, -.2f};
    if (tData.size() < 2) tData = {time, time + 1500000};
    if (time >= tData[1]) {
        gui.setDestroyed();
        return glm::vec2();
    }
    float progress = (float)(time - tData[0]) / (tData[1] - tData[0]);
    float x = progress * data[0];
    float y = progress * data[1];
    glm::vec4 col = gui.getColor();
    col.a = 1.f - progress;
    gui.setColor(col);
    return glm::vec2({x, y});
}

glm::vec2 applyWavy(int64_t time, Gui& gui, std::vector<float>& data,
                    std::vector<int64_t>& tData) {
    if (data.size() < 2) data = {0.f, .2f};
    if (tData.size() < 2) tData = {time, 1000000};
    float progress =
        (float)((tData[0] - time) % (int64_t)(tData[1] * M_PI * 2)) / tData[1];
    float x = data[0] * std::sin(progress), y = data[1] * std::sin(progress);
    return glm::vec2({x, y});
}

// TODO replace this with a less heavy system
glm::vec2 applyOffset(int64_t time, Gui& gui, std::vector<float>& data,
                      std::vector<int64_t>& iData) {
    if (data.size() < 2) return glm::vec2();
    return glm::vec2({data[0], data[1]});
}