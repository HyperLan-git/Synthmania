#pragma once

class ParentedGui;

#include "Gui.hpp"

class ParentedGui : public Gui {
   public:
    ParentedGui(ImageView* texture, const char* name, Gui* parent);

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const;

    virtual ~ParentedGui() = default;

   private:
    Gui* parent;
};