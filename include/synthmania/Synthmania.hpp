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

#include <iostream>
#include <stdexcept>

#include "AudioHandler.hpp"
#include "AudioPluginHandler.hpp"
#include "Entity.hpp"
#include "Gui.hpp"
#include "JsonHandler.hpp"
#include "MidiHandler.hpp"
#include "Note.hpp"
#include "Precision.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

class Synthmania {
   public:
    Synthmania(std::string songfolder, std::string skin);

    void run();
    void update();

    std::map<std::string, std::string> readTextures(std::string skin);
    std::map<std::string, std::string> getTextures();

    Renderer *getRenderer();
    AudioPluginHandler *getPluginHandler();

    std::vector<Entity *> getEntities();
    std::vector<Gui *> getGuis();

    Window *getWindow();

    void addGui(Gui *gui);
    void addEntity(Entity *entity);

    int64_t getCurrentTimeMillis();

    ~Synthmania();

   private:
    MidiHandler *handler;
    Window *window;
    Renderer *renderer;
    AudioHandler *audio;
    AudioPluginHandler *plugin;
    TrackPartition partition;

    std::chrono::_V2::system_clock::time_point begTime =
        std::chrono::high_resolution_clock::now();
    uint64_t startTime = 0;

    std::map<std::string, std::string> textures;

    std::vector<Entity *> entities;
    std::vector<Gui *> guis;
    std::vector<Note *> notes;
    AudioSource *music;
};