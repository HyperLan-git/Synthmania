#pragma once

class Judgement;

#include "MidiNote.hpp"
#include "PartitionNotation.hpp"

// The crimes thy kind have committed are not forgotten.
// And thy punishment... is death!
class Judgement : public Gui {
   public:
    Judgement(std::string name, Texture texture, TrackPartition notes);

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const { return Gui::getShaderData(); }

   private:
    TrackPartition partition;
};