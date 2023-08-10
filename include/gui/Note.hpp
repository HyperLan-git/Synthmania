#pragma once

class Note;

#include "MidiNote.hpp"
#include "PartitionNotation.hpp"

#define HIT_WINDOW 150000
#define DRUM_HIT_WINDOW 150000
#define DELETE_ANIM 50000

enum NoteStatus {
    WAITING,   // IDLE
    HIT,       // SUCCESSFULLY HIT
    FINISHED,  // FADING AWAY
    MISSED     // FAILED
};

unsigned char transposePitch(Key k, unsigned char pitch);

std::vector<double> splitDuration(double duration);

Texture getTextureForNote(unsigned char pitch, double duration, Key currentKey);

glm::vec2 getSizeAndLocForNote(double duration, Key k, unsigned char pitch);

class Note : public PartitionNotation {
   public:
    Note(std::string name, int64_t time, unsigned char pitch,
         double totalDuration, double duration, uint64_t MPQ, Key key,
         KeySignature signature);

    void setStatus(NoteStatus status);

    bool justMissed() const;
    NoteStatus getStatus() const;
    int64_t getTime() const;
    int64_t getDuration() const;
    int64_t getTotalDuration() const;
    unsigned char getPitch() const;
    void kill(uint64_t moment);

    virtual bool update(int64_t time);

   private:
    int64_t totalDuration, duration;
    int64_t kill_moment;
    unsigned char pitch;
    NoteStatus status = WAITING;
    bool missed = false;
};