#include "Precision.hpp"

Precision::Precision(ImageView* texture, const char* name, int64_t time,
                     int64_t delta)
    : Gui(texture, name) {
    hit_time = time;
    this->delta = delta;
}

bool Precision::update(int64_t time) {
    uint64_t d = this->hit_time + LINGER_PRECISION - time;
    color.a = d / (double)LINGER_PRECISION;
    this->position.x = delta / (double)HIT_WINDOW;

    return this->hit_time + LINGER_PRECISION < time;
}

ShaderData* Precision::getShaderData() const {
    ShaderData* data = new ShaderData();
    GuiData* edata = (GuiData*)malloc(sizeof(GuiData));
    glm::vec3 p = glm::vec3(graphicalPosition, 0);
    p += position;
    edata->pos = p;
    edata->rot = rotation;
    edata->size = size;
    edata->color = color;
    data->data = edata;
    data->size = sizeof(GuiData);
    return data;
}