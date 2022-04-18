#pragma once

class Note;

#include "Gui.hpp"
#include "ImageView.hpp"
#include "Utils.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <map>
#include <string>

enum Key { SOL, FA };

ImageView* getTextureForNote(std::vector<ImageView*> textures, u_char pitch,
                             float duration, Key currentKey);

class Note : public Gui {
   public:
    Note(const char* name, float time, u_char pitch, float duration,
         std::vector<ImageView*> textures);

    virtual bool update(float time);

    virtual ShaderData* getShaderData() const;

   private:
    float time;
    u_char pitch;
};