#pragma once

class Synthmania;

#define GLFW_INCLUDE_VULKAN
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include <GLFW/glfw3.h>
#include <stdlib.h>

#include <iostream>
#include <stdexcept>

#include "AudioHandler.hpp"
#include "Entity.hpp"
#include "Gui.hpp"
#include "MidiHandler.hpp"
#include "Note.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

class Synthmania {
   public:
    Synthmania();

    void run();
    void update();

    std::vector<Entity *> getEntities();
    std::vector<Gui *> getGuis();

    void addGui(Gui *gui);
    void addEntity(Entity *entity);

    int64_t getCurrentTimeMillis();

    ~Synthmania();

   private:
    MidiHandler *handler;
    Window *window;
    Renderer *renderer;
    AudioHandler *audio;
    TrackPartition partition;

    std::chrono::_V2::system_clock::time_point begTime =
        std::chrono::high_resolution_clock::now();
    uint64_t startTime = 0;

    std::vector<Entity *> entities;
    std::vector<Gui *> guis;
    std::vector<Note *> notes;
};