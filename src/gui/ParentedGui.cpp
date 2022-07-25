#include "ParentedGui.hpp"

ParentedGui::ParentedGui(ImageView* texture, const char* name, Gui* parent)
    : Gui(texture, name) {
    this->parent = parent;
}

bool ParentedGui::update(int64_t time) { return parent->isDestroyed(); }

ShaderData* ParentedGui::getShaderData() const {
    ShaderData* data = new ShaderData();
    GuiData* edata = (GuiData*)malloc(sizeof(GuiData));
    float rot = parent->getRotation();
    glm::vec3 p = glm::vec3(graphicalPosition, 0);
    p += parent->getPosition();
    edata->pos =
        p + glm::vec3(position.x * glm::cos(rot) - position.y * glm::sin(rot),
                      position.y * glm::cos(rot) + position.x * glm::sin(rot),
                      position.z - parent->getPosition().z);
    edata->rot = rotation + rot;
    edata->size = size;
    glm::vec4 color = parent->getColor();
    if (parent->getNegate() || this->negate) color.a *= -1;
    edata->color = color;
    data->data = edata;
    data->size = sizeof(GuiData);
    return data;
}

glm::vec2 ParentedGui::getRealPosition() const {
    glm::vec3 result = position;
    result += this->parent->getPosition();
    return {result.x, result.y};
}