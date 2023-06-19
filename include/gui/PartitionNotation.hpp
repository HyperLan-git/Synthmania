#pragma once

#include "Gui.hpp"
#include "MidiNote.hpp"

enum Key { SOL, FA, DRUM };

enum Accidental { NONE, NATURAL, SHARP, FLAT };

KeySignature toMajor(const KeySignature& signature);

int getDifferenceFromC4(unsigned char pitch, KeySignature signature);

Accidental getAccidental(unsigned char pitch, KeySignature signature);
bool isFromCMajor(unsigned char pitch);

int getOffset(Key key);

class PartitionNotation : public Gui {
   public:
    PartitionNotation(std::string name, int64_t time, unsigned char pitch,
                      ImageView* texture, Key key, KeySignature signature);

    PartitionNotation(std::string name, int64_t time, double verticalPosition,
                      ImageView* texture);

    int64_t getTime();

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const { return Gui::getShaderData(); }

   protected:
    int64_t time;
};