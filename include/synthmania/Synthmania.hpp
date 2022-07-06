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
#include "MidiHandler.hpp"
#include "Note.hpp"
#include "Precision.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

class Synthmania : public Game {
   public:
    Synthmania(std::string songfolder, std::string skin);

    virtual void init();

    virtual void update();

    AudioPluginHandler *getPluginHandler();

    virtual void setTimeMicros(int64_t time);

    ~Synthmania();

    static void keyCallback(GLFWwindow *win, int key, int scancode, int action,
                            int mods);

   private:
    MidiHandler *handler;
    AudioHandler *audio;
    AudioPluginHandler *plugin;
    TrackPartition partition;

    std::vector<Note *> notes;
    AudioSource *music;
    std::string songFolder, skin;
};