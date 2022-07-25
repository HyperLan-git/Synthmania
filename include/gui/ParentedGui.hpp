#pragma once

class ParentedGui;

#include "Gui.hpp"

// TODO fix with shared ptr or smth

/**
 * @brief A gui pointing to another, that moves with it and lives with it
 * Because it gets deleted whenever the parent dies it should ALWAYS be added
 * after the parent
 */
class ParentedGui : public Gui {
   public:
    ParentedGui(ImageView* texture, const char* name, Gui* parent);

    virtual glm::vec2 getRealPosition() const;

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const;

   private:
    Gui* parent;
};