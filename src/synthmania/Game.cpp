#include "Game.hpp"

#include "MainMenu.hpp"
#include "Renderer.hpp"

Game::Game() {
    textures = std::map<std::string, std::string>();
    textures.emplace("missing", "resources/textures/missing.jpg");
    const char *list =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 "
        "&\"'(-_)=+|#{}[]`\\^@*,;:!?./%$";
    std::vector<unsigned long> chars;
    for (const char *c = list; *c != '\0'; c++) chars.push_back(*c);
    fontsToLoad.emplace("resources/fonts/Stupid.ttf", chars);
}

void Game::init() {
    renderer->loadTextures(textures, fontsToLoad);
    window->setWindowUserPointer(this);
    this->menus.emplace("main", new MainMenu(this));
}

void Game::resetScene() {
    if (menu == NULL)
        for (Gui *g : guis) delete g;
    guis.clear();
    menu = NULL;
    for (Entity *e : entities) delete e;
    entities.clear();
}

void Game::loadMenu(std::string m) {
    this->menu = this->menus[m];
    menu->show();
}

void Game::run() {
    setTimeMicros(-this->startTime);
    while (!window->shouldClose()) {
        glfwPollEvents();
        glm::vec2 pos = renderer->getVirtPos(window->getCursorPos());

        if (menu != NULL) {
            bool pressed = window->mousePressed();
            for (Button *b : menu->getButtons())
                if (!pressed) {
                    if (b->isPressed() && b->isInside(pos)) {
                        menu->onPressed(b);
                        b->onReleased();
                        break;
                    }
                    b->onReleased();
                } else if (b->isInside(pos) && !b->isPressed()) {
                    b->onPressed();
                    break;
                }
        }
        if (audio != NULL) audio->update();
        update();
        if (menu != NULL) menu->update(getCurrentTimeMicros());
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

void Game::setRenderer(Renderer *renderer) { this->renderer = renderer; }
void Game::setWindow(Window *window) { this->window = window; }

Window *Game::getWindow() { return this->window; }

Renderer *Game::getRenderer() { return renderer; }

size_t Game::updateUBO(void *&ubo) { return sizeof(UniformBufferObject); }
void Game::freeUBO(void *&ubo) {}

size_t Game::updateFinalUBO(void *&ubo) { return sizeof(UniformBufferObject); }
void Game::freeFinalUBO(void *&ubo) {}

void Game::addEntity(Entity *entity) { entities.push_back(entity); }

float prevZ = 0.9999f;

void Game::addGui(Gui *gui) {
    if (guis.empty()) {
        gui->setZ(0.9999f);
        prevZ = 0.9999f;
    } else {
        gui->setZ(prevZ -= 0.00001f);
    }
    this->guis.push_back(gui);
}

std::vector<Entity *> Game::getEntities() { return entities; }
std::vector<Gui *> Game::getGuis() { return guis; }

void Game::playSound(std::string sound) { this->audio->playSound(sound); }

Game::~Game() {
    resetScene();

    for (auto entry : menus) delete entry.second;

    delete audio;

    delete renderer;
    delete window;

    glfwTerminate();
}
