#pragma once

#include "Gui.hpp"

int getDifferenceFromC4(u_char pitch);

bool isFromCMajor(u_char pitch);

class PartitionNotation : public Gui {
   public:
    PartitionNotation(const char* name, int64_t time, u_char pitch,
                      ImageView* texture);

    PartitionNotation(const char* name, int64_t time, double verticalPosition,
                      ImageView* texture);

    int64_t getTime();

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const;

   protected:
    int64_t time;
};