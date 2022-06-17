#pragma once

class Precision;

#include "Gui.hpp"
#include "Note.hpp"

#define LINGER_PRECISION 2000000

class Precision : public Gui {
   public:
    Precision(ImageView* texture, const char* name, int64_t time,
              int64_t delta);

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const;

   private:
    int64_t hit_time;
    int64_t delta;
};