#pragma once

class Precision;

#include "Gui.hpp"
#include "Note.hpp"

#define LINGER_PRECISION 2000000

class Precision : public Gui {
   public:
    Precision(Texture texture, std::string name, int64_t time, int64_t delta,
              int64_t hit_window = HIT_WINDOW);

    virtual bool update(int64_t time);

   private:
    int64_t hit_time;
    int64_t delta;
};