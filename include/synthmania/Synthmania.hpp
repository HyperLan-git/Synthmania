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
#include <stdexcept>

#include "AudioHandler.hpp"
#include "AudioPluginHandler.hpp"
#include "Entity.hpp"
#include "Game.hpp"
#include "Gui.hpp"
#include "JsonHandler.hpp"
#include "Judgement.hpp"
#include "MidiHandler.hpp"
#include "Note.hpp"
#include "Precision.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

#define HIT_TIME 1500000

std::vector<Gui *> printString(std::string text, Renderer *renderer,
                               std::string entityNames, std::string font,
                               double size, glm::vec2 pos,
                               glm::vec4 color = {0, 0, 0, 1});

std::vector<Gui *> printShadowedString(std::string text, Renderer *renderer,
                                       std::string entityNames,
                                       std::string font, double size,
                                       glm::vec2 pos,
                                       glm::vec4 color = {0, 0, 0, 1});

std::vector<Gui *> printShakingString(std::string text, Renderer *renderer,
                                      std::string entityNames, std::string font,
                                      double size, glm::vec2 pos, float shake,
                                      glm::vec4 color = {0, 0, 0, 1});

class Synthmania : public Game {
   public:
    Synthmania(std::string songfolder, std::string skin);

    virtual void init();

    virtual void update();

    AudioPluginHandler *getPluginHandler();

    virtual void setTimeMicros(int64_t time);

    void noteHit(Note *note);
    void noteMiss(Note *note);

    Chart getChart();
    TrackPartition getPartition();

    virtual size_t updateUBO(void *&ubo);

    virtual void addGui(Gui *gui);

    ~Synthmania();

    static void keyCallback(GLFWwindow *win, int key, int scancode, int action,
                            int mods);

   private:
    MidiHandler *handler;
    AudioHandler *audio;
    AudioPluginHandler *plugin;
    TrackPartition partition;
    Chart chart;
    Diff diff;
    GraphicalEffectHandler *mod = NULL;

    std::vector<Note *> notes;
    AudioSource *music;
    std::string songFolder, skin;
    Judgement *line;
    bool autoPlay = false;
};