#pragma once

class Note;

#include "ImageView.hpp"
#include "MidiNote.hpp"
#include "PartitionNotation.hpp"
#include "Utils.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <map>
#include <string>

#define HIT_WINDOW 150000
#define DRUM_HIT_WINDOW 100000
#define DELETE_ANIM 50000

enum NoteStatus {
    WAITING,   // IDLE
    HIT,       // SUCCESSFULLY HIT
    FINISHED,  // FADING AWAY
    MISSED     // FAILED
};

unsigned char transposePitch(Key k, unsigned char pitch);

std::vector<double> splitDuration(double duration);

ImageView* getTextureForNote(std::vector<ImageView*> textures,
                             unsigned char pitch, double duration,
                             Key currentKey);

glm::vec2 getSizeAndLocForNote(double duration, Key k, unsigned char pitch);

class Note : public PartitionNotation {
   public:
    Note(std::string name, int64_t time, unsigned char pitch,
         double totalDuration, double duration, uint64_t MPQ,
         std::vector<ImageView*> textures, Key key);

    bool justMissed();
    void setStatus(NoteStatus status);
    NoteStatus getStatus();
    int64_t getTime();
    int64_t getDuration();
    int64_t getTotalDuration();
    unsigned char getPitch();
    void kill(uint64_t moment);

    virtual bool update(int64_t time);

    virtual ShaderData* getShaderData() const { return Gui::getShaderData(); }

   private:
    int64_t totalDuration, duration;
    int64_t kill_moment;
    unsigned char pitch;
    NoteStatus status = WAITING;
    bool missed = false;
};