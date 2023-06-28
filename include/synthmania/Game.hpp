#pragma once

class Game;

#include "AudioHandler.hpp"
#include "Entity.hpp"
#include "Gui.hpp"
#include "JsonHandler.hpp"
#include "Menu.hpp"
#include "MidiHandler.hpp"
#include "Options.hpp"
#include "Precision.hpp"
#include "TextHandler.hpp"
#include "Window.hpp"

class Renderer;

// TODO pass everything that needs it by reference this is so much
// useless copying
class Game {
   public:
    Game();

    virtual void init();

    void run();
    virtual void update() = 0;

    std::map<std::string, std::string> readTextures(std::string file);
    std::map<std::string, std::string> getTextures();

    void setRenderer(Renderer *renderer);
    void setWindow(Window *window);

    Renderer *getRenderer();

    std::vector<std::shared_ptr<Entity>> getEntities();
    std::vector<std::shared_ptr<Gui>> getGuis();

    virtual void resetScene();
    virtual void loadMenu(std::string menu);

    Window *getWindow();

    TextHandler *getTextHandler();

    virtual size_t updateUBO(void *&ubo);
    virtual void freeUBO(void *&ubo);

    virtual size_t updateFinalUBO(void *&ubo);
    virtual void freeFinalUBO(void *&ubo);

    template <typename T>
    void addTGui(std::shared_ptr<T> &gui) {
        std::shared_ptr<Gui> temp = std::dynamic_pointer_cast<Gui>(gui);
        addGui(temp);
    }
    virtual void addGui(std::shared_ptr<Gui> &gui);
    virtual void addEntity(std::shared_ptr<Entity> &entity);

    Menu *getMenu(std::string menu);
    Menu *getCurrentMenu();

    void playSound(std::string sound);

    int64_t getCurrentTimeMicros();
    virtual void setTimeMicros(int64_t time);
    void resetClock();

    virtual ~Game();

   protected:
    GLFWkeyfun keyFunction = NULL;
    Window *window;
    Renderer *renderer;

    AudioHandler *audio;

    std::chrono::_V2::system_clock::time_point begTime =
        std::chrono::high_resolution_clock::now();
    uint64_t startTime = 0, relativeTime = 0;

    std::map<std::string, std::string> textures;
    std::map<std::string, std::vector<unsigned long>> fontsToLoad;

    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<Gui>> guis;

    // What will I serve you today sir ?
    Menu *menu = NULL;
    std::map<std::string, Menu *> menus;
};