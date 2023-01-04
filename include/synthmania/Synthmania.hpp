#pragma once

class Synthmania;

// TODO remove all these defines
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
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
#include "PlayMode.hpp"
#include "Precision.hpp"
#include "Renderer.hpp"
#include "SongSelectMenu.hpp"
#include "Window.hpp"

#define HIT_TIME 1500000
// TODO move this elsewhere
std::vector<std::shared_ptr<Gui>> printString(std::string text,
                                              TextHandler *textHandler,
                                              std::string entityNames,
                                              std::string font, double size,
                                              glm::vec2 pos,
                                              glm::vec4 color = {0, 0, 0, 1});

std::vector<std::shared_ptr<Gui>> printShadowedString(
    std::string text, TextHandler *textHandler, std::string entityNames,
    std::string font, double size, glm::vec2 pos,
    glm::vec4 color = {0, 0, 0, 1});

std::vector<std::shared_ptr<Gui>> printShakingString(
    std::string text, TextHandler *textHandler, std::string entityNames,
    std::string font, double size, glm::vec2 pos, float shake,
    glm::vec4 color = {0, 0, 0, 1});

// TODO check for note length
// TODO merge note flags together
class Synthmania : public Game {
   public:
    Synthmania(std::string skin, std::string config);

    void loadSong(std::string songFolder);

    void setGamemode(Gamemode *gamemode);

    void resetAudio();

    virtual void init();

    virtual void update();

#ifndef NOVST
    AudioPluginHandler *getPluginHandler();
#endif

    virtual void setTimeMicros(int64_t time);

    int64_t getStartTime();

    Options *getOptions();
    MidiHandler *getMidiHandler();
    AudioHandler *getAudioHandler();

    float getMusicVolume();
    int64_t getAudioLeniency();

    virtual void resetScene();

    virtual size_t updateUBO(void *&ubo);
    virtual void freeUBO(void *&ubo);

    virtual size_t updateFinalUBO(void *&ubo);
    virtual void freeFinalUBO(void *&ubo);

    virtual void addGui(std::shared_ptr<Gui> &gui);

#ifndef NOVST
    std::string getPlugin(std::string plugin);
#endif
    Gamemode *getGamemode();

    bool isFullscreen();

    void applyOptions();

    ~Synthmania();

   private:
    MidiHandler *handler;

    Gamemode *gamemode = NULL;

    std::string skin;
    std::map<unsigned char, unsigned char> midiMapping;
    std::unique_ptr<Options> options;
    float musicVol = 1;
    int64_t audioLatency = 0, graphicalLatency = 0, audioLeniency = 20000;
    bool fullscreen = false, playing = false;

#ifndef NOVST
    int bufSize = 1024, bufAmt = 3;
    std::vector<std::string> pluginFolders = {"./plugins"};
    std::map<std::string, std::string> availablePlugins;
#endif
};