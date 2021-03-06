#pragma once

#include "Gui.hpp"

int getDifferenceFromC4(unsigned char pitch);

bool isFromCMajor(unsigned char pitch);

class PartitionNotation : public Gui {
   public:
    PartitionNotation(const char* name, int64_t time, unsigned char pitch,
                      ImageView* texture);

    PartitionNotation(const char* name, int64_t time, double verticalPosition,
                      ImageView* texture);

    int64_t getTime();

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const { return Gui::getShaderData(); }

   protected:
    int64_t time;
};