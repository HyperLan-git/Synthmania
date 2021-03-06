#pragma once

class Game;

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

#include "Entity.hpp"
#include "Gui.hpp"
#include "JsonHandler.hpp"
#include "MidiHandler.hpp"
#include "Precision.hpp"
#include "Window.hpp"

class Renderer;

class Game {
   public:
    Game();

    virtual void init() = 0;  // Called after textures are loaded

    void run();
    virtual void update() = 0;

    std::map<std::string, std::string> readTextures(std::string file);
    std::map<std::string, std::string> getTextures();

    void setRenderer(Renderer *renderer);
    void setWindow(Window *window);

    Renderer *getRenderer();

    std::vector<Entity *> getEntities();
    std::vector<Gui *> getGuis();

    Window *getWindow();

    virtual size_t updateUBO(void *&ubo);
    virtual void freeUBO(void *&ubo);

    virtual void addGui(Gui *gui);
    virtual void addEntity(Entity *entity);

    int64_t getCurrentTimeMicros();
    virtual void setTimeMicros(int64_t time);

    ~Game();

   protected:
    GLFWkeyfun keyFunction = NULL;
    Window *window;
    Renderer *renderer;

    std::chrono::_V2::system_clock::time_point begTime =
        std::chrono::high_resolution_clock::now();
    uint64_t startTime = 0, relativeTime = 0;

    std::map<std::string, std::string> textures;
    std::map<std::string, std::vector<unsigned long>> fontsToLoad;

    std::vector<Entity *> entities;
    std::vector<Gui *> guis;
};