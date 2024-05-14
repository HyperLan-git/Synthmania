#pragma once

class PlayMode;

class ChartHandler;
class Synthmania;

#include "AudioPluginHandler.hpp"
#include "AudioSource.hpp"
#include "Chart.hpp"
#include "Gamemode.hpp"
#include "Judgement.hpp"
#include "MidiHandler.hpp"
#include "MidiNote.hpp"
#include "Note.hpp"
#include "Score.hpp"
#include "Utils.hpp"

class PlayMode : public Gamemode {
   public:
    PlayMode(Synthmania &game, std::string songFolder);

    virtual bool update() override;

    void noteHit(const std::shared_ptr<Note> &note);
    void noteMiss(const std::shared_ptr<Note> &note);

    Chart getChart();
    TrackPartition getPartition();

    std::string getSongFolder();

    void playDrumSound(unsigned char pitch);
    void playPianoSound(unsigned char pitch);

    void spawnNote(MidiNote note);

    virtual void onSpawn(std::shared_ptr<Gui> &g) override;
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
    Synthmania &game;

    TrackPartition partition;
    Chart chart;
    Diff diff;
    // Discord reference
    std::unique_ptr<ChartHandler> mod;

    std::vector<std::weak_ptr<Note>> notes;
    std::optional<std::reference_wrapper<AudioSource>> music;
    std::string songFolder;
    std::vector<std::shared_ptr<Gui>> autoplayText;
    int64_t start;
    std::vector<Message> replay;
    std::shared_ptr<Judgement> line;
    bool autoPlay = false, drum = false;

    Score score;
    uint32_t score_per_note;

#ifdef VST
    AudioPluginHandler *plugin = NULL;
#endif
};

#include "ChartHandler.hpp"
#include "Synthmania.hpp"