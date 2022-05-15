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

#define HIT_WINDOW 100000
#define DELETE_ANIM 200000

enum Key { SOL, FA };

enum NoteStatus { WAITING, HIT, MISSED };

ImageView* getTextureForNote(std::vector<ImageView*> textures, u_char pitch,
                             double duration, Key currentKey);

int getDifferenceFromC4(u_char pitch);

bool isFromCMajor(u_char pitch);

glm::vec2 getSizeAndLocForNote(double duration);

class Note : public Gui {
   public:
    Note(const char* name, int64_t time, u_char pitch, double duration,
         std::vector<ImageView*> textures);

    void setStatus(NoteStatus status);
    int64_t getTime();
    u_char getPitch();

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const;

   private:
    int64_t time;
    int64_t kill_moment;
    u_char pitch;
    NoteStatus status = WAITING;
};