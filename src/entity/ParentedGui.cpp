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
    edata->pos =
        parent->getPosition() +
        glm::vec3(position.x * glm::cos(rot) - position.y * glm::sin(rot),
                  position.y * glm::cos(rot) + position.x * glm::sin(rot),
                  position.z - parent->getPosition().z);
    edata->rot = rotation + rot;
    edata->size = size;
    edata->color = parent->getColor();
    data->data = edata;
    data->size = sizeof(GuiData);
    return data;
}

ParentedGui::~ParentedGui() {}