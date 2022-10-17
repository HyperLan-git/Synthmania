#pragma once

class Synthmania;

#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include <stdlib.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "AudioHandler.hpp"
#ifndef NOVST
#include "AudioPluginHandler.hpp"
#endif
#include "Entity.hpp"
#include "Game.hpp"
#include "Gui.hpp"
#include "JsonHandler.hpp"
#include "Judgement.hpp"
#include "MainMenu.hpp"
#include "MidiHandler.hpp"
#include "Note.hpp"
#include "OptionMenu.hpp"
#include "Options.hpp"
#include "Precision.hpp"
#include "Renderer.hpp"
#include "SongSelectMenu.hpp"
#include "Window.hpp"

#define HIT_TIME 1500000

std::vector<Gui *> printString(std::string text, TextHandler *textHandler,
                               std::string entityNames, std::string font,
                               double size, glm::vec2 pos,
                               glm::vec4 color = {0, 0, 0, 1});

std::vector<Gui *> printShadowedString(std::string text,
                                       TextHandler *textHandler,
                                       std::string entityNames,
                                       std::string font, double size,
                                       glm::vec2 pos,
                                       glm::vec4 color = {0, 0, 0, 1});

std::vector<Gui *> printShakingString(std::string text,
                                      TextHandler *textHandler,
                                      std::string entityNames, std::string font,
                                      double size, glm::vec2 pos, float shake,
                                      glm::vec4 color = {0, 0, 0, 1});

class Synthmania : public Game {
   public:
    Synthmania(std::string skin, std::string config);

    void loadSong(std::string songFolder);

    virtual void init();

    virtual void update();

#ifndef NOVST
    AudioPluginHandler *getPluginHandler();
#endif

    virtual void setTimeMicros(int64_t time);

    void noteHit(Note *note);
    void noteMiss(Note *note);

    Chart getChart();
    TrackPartition getPartition();
    Options* getOptions();

    std::string getSongFolder();

    virtual void resetScene();

    virtual size_t updateUBO(void *&ubo);
    virtual void freeUBO(void *&ubo);

    virtual size_t updateFinalUBO(void *&ubo);
    virtual void freeFinalUBO(void *&ubo);

    virtual void addGui(Gui *gui);

    ~Synthmania();

    static void keyCallback(GLFWwindow *win, int key, int scancode, int action,
                            int mods);

   private:
    MidiHandler *handler;
#ifndef NOVST
    AudioPluginHandler *plugin = NULL;
#endif
    TrackPartition partition;
    Chart chart;
    Diff diff;
    ChartHandler *mod = NULL;

    std::map<unsigned char, unsigned char> midiMapping;
    std::vector<Note *> notes;
    AudioSource *music = NULL;
    std::string songFolder, skin;
    Judgement *line = NULL;
    bool autoPlay = false, drum = false;
    std::unique_ptr<Options> options;
};