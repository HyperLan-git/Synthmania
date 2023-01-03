#pragma once

class PlayMode;

#include "AudioPluginHandler.hpp"
#include "AudioSource.hpp"
#include "Chart.hpp"
#include "Gamemode.hpp"
#include "Judgement.hpp"
#include "MidiNote.hpp"
#include "Note.hpp"
#include "Utils.hpp"
class ChartHandler;
class Synthmania;

class PlayMode : public Gamemode {
   public:
    PlayMode(Synthmania *game, std::string songFolder);

    virtual bool update() override;

    void noteHit(Note *note);
    void noteMiss(Note *note);

    Chart getChart();
    TrackPartition getPartition();

    std::string getSongFolder();

    void playDrumSound(unsigned char pitch);
    void playPianoSound(unsigned char pitch);

    void spawnNote(MidiNote note);

    virtual void onSpawn(Gui *g) override;
    virtual void onClockAdjust(int64_t value) override;
    virtual void onConfigChange() override;

    virtual size_t updateUBO(void *&ubo) override;
    virtual void freeUBO(void *&ubo) override;

    virtual size_t updateFinalUBO(void *&ubo) override;
    virtual void freeFinalUBO(void *&ubo) override;

    virtual ~PlayMode();

    static void keyCallback(GLFWwindow *win, int key, int scancode, int action,
                            int mods);

   private:
    Synthmania *game;

    TrackPartition partition;
    Chart chart;
    Diff diff;
    // Discord reference
    ChartHandler *mod = NULL;

    std::vector<Note *> notes;
    AudioSource *music = NULL;
    std::string songFolder;
    Judgement *line = NULL;
    bool autoPlay = false, drum = false;

#ifndef NOVST
    AudioPluginHandler *plugin = NULL;
#endif
};

#include "ChartHandler.hpp"