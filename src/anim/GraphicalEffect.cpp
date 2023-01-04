#include "GraphicalEffect.hpp"

GraphicalEffect::GraphicalEffect(GraphicalEffectFunction function, float* data,
                                 int64_t* tData) {
    this->function = function;
    this->data = data;
    this->tData = tData;
}

glm::vec2 GraphicalEffect::operator()(Gui* gui, int64_t time) {
    return this->function(time, gui, data, tData);
}

GraphicalEffect::~GraphicalEffect() {
    if (data != NULL) delete[] data;
    if (tData != NULL) delete[] tData;
}

glm::vec2 applyShaking(int64_t time, Gui* gui, float*& data, int64_t*& tData) {
    if (data == NULL) data = new float[1]{.1f};
    float amplitude = *(float*)data;
    float r = std::rand() * amplitude / RAND_MAX - amplitude / 2.,
          r2 = std::rand() * amplitude / RAND_MAX - amplitude / 2.;
    return glm::vec2({r, r2});
}

glm::vec2 applyTemp(int64_t time, Gui* gui, float*& data, int64_t*& tData) {
    if (data == NULL) data = new float[2]{0.f, -.2f};
    if (tData == NULL) tData = new int64_t[2]{time, time + 1500000};
    if (time >= tData[1]) {
        gui->setDestroyed();
        return glm::vec2();
    }
    float progress = (float)(time - tData[0]) / (tData[1] - tData[0]);
    float x = progress * data[0];
    float y = progress * data[1];
    glm::vec4 col = gui->getColor();
    col.a = 1.f - progress;
    gui->setColor(col);
    return glm::vec2({x, y});
}

// TODO replace this with a less heavy system
glm::vec2 applyOffset(int64_t time, Gui* gui, float*& data, int64_t*& iData) {
    if (data == NULL) return glm::vec2();
    return glm::vec2({data[0], data[1]});
}