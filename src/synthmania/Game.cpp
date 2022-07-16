#include "Game.hpp"

Game::Game(int width, int height, const char *title) {
    window = new Window(width, height, title);
    renderer = new Renderer(this, window);
    textures = std::map<std::string, std::string>();
    textures.emplace("missing", "resources/textures/missing.jpg");
    const char *list =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 "
        "&\"'(-_)=+|#{}[]`\\^@*,;:!?./%$";
    std::vector<unsigned long> chars;
    for (const char *c = list; *c != '\0'; c++) chars.push_back(*c);
    fontsToLoad.emplace("resources/fonts/Stupid.ttf", chars);
}

void Game::run() {
    renderer->loadTextures(textures, fontsToLoad);
    window->setWindowUserPointer(this);
    init();
    if (keyFunction != NULL) window->setKeycallback(keyFunction);
    setTimeMicros(-this->startTime);
    while (!window->shouldClose()) {
        glfwPollEvents();
        update();
        renderer->render();
    }
}

void Game::setTimeMicros(int64_t time) {
    relativeTime -= time - getCurrentTimeMicros();
}

std::map<std::string, std::string> Game::readTextures(std::string file) {
    std::map<std::string, std::string> result;
    tree *t = readJson(file.c_str());
    readTree(*t, result, "");
    delete t;
    return result;
}

int64_t Game::getCurrentTimeMicros() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    // Necessary conversions to not lose precision
    return std::chrono::duration<int64_t, std::chrono::nanoseconds::period>(
               currentTime - begTime)
                   .count() /
               (uint64_t)1000 -
           (int64_t)this->startTime - (int64_t)relativeTime;
}

Window *Game::getWindow() { return this->window; }

Renderer *Game::getRenderer() { return renderer; }

void Game::addEntity(Entity *entity) { entities.push_back(entity); }

void Game::addGui(Gui *gui) {
    if (guis.empty()) {
        gui->setZ(0.9999f);
    } else {
        gui->setZ((*--guis.end())->getPosition().z - 0.00001f);
    }
    this->guis.push_back(gui);
}

std::vector<Entity *> Game::getEntities() { return entities; }
std::vector<Gui *> Game::getGuis() { return guis; }

Game::~Game() {
    for (Entity *e : entities) delete e;
    for (Gui *g : guis) delete g;

    delete renderer;
    delete window;

    glfwTerminate();
}
