#include "ParentedGui.hpp"

ParentedGui::ParentedGui(Texture texture, std::string name,
                         std::weak_ptr<Gui> parent)
    : Gui(texture, name) {
    this->parent = parent;
}

bool ParentedGui::update(int64_t time) {
    if (parent.expired()) return true;
    return std::shared_ptr<Gui>(parent)->isDestroyed();
}

ShaderData* ParentedGui::getShaderData() const {
    if (parent.expired()) return Gui::getShaderData();
    ShaderData* data = new ShaderData();
    GuiData* edata = (GuiData*)malloc(sizeof(GuiData));
    std::shared_ptr<Gui> par(parent);
    float rot = par->getRotation();
    glm::vec3 p = glm::vec3(graphicalPosition, 0);
    p += par->getPosition();
    edata->pos =
        p + glm::vec3(position.x * glm::cos(rot) - position.y * glm::sin(rot),
                      position.y * glm::cos(rot) + position.x * glm::sin(rot),
                      position.z - par->getPosition().z);
    edata->rot = rotation + rot;
    edata->size = size;
    glm::vec4 color = par->getColor();
    color.r *= this->color.r;
    color.g *= this->color.g;
    color.b *= this->color.b;
    if (par->getNegate() != this->negate) color.a *= -1;
    edata->color = color;
    data->data = edata;
    data->size = sizeof(GuiData);
    return data;
}

glm::vec2 ParentedGui::getRealPosition() const {
    if (parent.expired()) return Gui::getRealPosition();
    glm::vec3 result = position;
    result += std::shared_ptr<Gui>(this->parent)->getPosition();
    return {result.x, result.y};
}