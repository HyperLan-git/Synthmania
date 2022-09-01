#pragma once

class Judgement;

#include "Gui.hpp"
#include "ImageView.hpp"
#include "MidiNote.hpp"
#include "Note.hpp"
#include "Utils.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <map>
#include <string>

class Judgement : public Gui {
   public:
    Judgement(std::string name, std::vector<ImageView*> textures,
              TrackPartition notes);

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const { return Gui::getShaderData(); }

   private:
    TrackPartition partition;
};