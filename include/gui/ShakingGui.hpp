#pragma once

#include <random>

#include "Gui.hpp"

class ShakingGui : public Gui {
   public:
    ShakingGui(ImageView* texture, const char* name, double amplitude);

    virtual bool update(int64_t time);

   private:
    double amplitude;
};