#pragma once

class ParentedGui;

#include <memory>

#include "Gui.hpp"

/**
 * @brief A gui pointing to another, that moves with it and lives with it.
 * When the parent dies, this also does afterwards.
 */
class ParentedGui : public Gui {
   public:
    ParentedGui(ImageView* texture, std::string name,
                std::weak_ptr<Gui> parent);

    virtual glm::vec2 getRealPosition() const;

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const;

   private:
    std::weak_ptr<Gui> parent;
};