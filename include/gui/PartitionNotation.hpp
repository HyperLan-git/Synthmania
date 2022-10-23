#pragma once

#include "Gui.hpp"

enum Key { SOL, FA, DRUM };

int getDifferenceFromC4(unsigned char pitch);

bool isFromCMajor(unsigned char pitch);

int getOffset(Key key);

class PartitionNotation : public Gui {
   public:
    PartitionNotation(std::string name, int64_t time, unsigned char pitch,
                      ImageView* texture, Key key);

    PartitionNotation(std::string name, int64_t time, double verticalPosition,
                      ImageView* texture);

    int64_t getTime();

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const { return Gui::getShaderData(); }

   protected:
    int64_t time;
};