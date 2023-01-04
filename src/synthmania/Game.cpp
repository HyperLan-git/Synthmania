#include "Game.hpp"

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
    CommandPool *pool = new CommandPool(renderer->getDevice());
    renderer->getTextHandler()->loadFonts(fontsToLoad, pool);
    renderer->loadTextures(textures);
    window->setWindowUserPointer(this);
    delete pool;
}

void Game::resetScene() {
    if (menu) menu->unselect();
    menu = NULL;
    guis.clear();
    entities.clear();
    window->resetCallbacks();
}

void Game::loadMenu(std::string m) {
    this->menu = this->menus[m];
    menu->show();
}

void Game::run() {
    bool clicked = false, lastPressed = false;
    while (!window->shouldClose()) {
        glfwPollEvents();

        // TODO handle everything with z values and do it elsewhere
        if (menu) {
            glm::vec2 pos = renderer->getVirtPos(window->getCursorPos());
            bool pressed = window->mousePressed();
            clicked = lastPressed && !pressed;
            lastPressed = pressed;
            const std::vector<std::shared_ptr<Button>> &buttons =
                menu->getButtons();
            const std::vector<std::shared_ptr<MenuElement>> &elements =
                menu->getMenuElements();
            if (pressed || clicked)
                for (auto iter = elements.begin(); iter != elements.end();
                     iter++) {
                    const std::shared_ptr<MenuElement> &e = *iter;
                    if (e->isInside(pos)) {
                        if (clicked) {
                            e->onClicked(pos);
                            menu->select(e);
                        } else if (pressed)
                            e->onPressed(pos);
                        break;
                    }
                }
            for (auto iter = buttons.rbegin(); iter != buttons.rend(); iter++) {
                std::shared_ptr<Button> b = *iter;
                if (clicked) {
                    if (b->isInside(pos)) {
                        menu->onPressed(b);
                        b->onReleased();
                        break;
                    }
                } else if (pressed && b->isInside(pos)) {
                    if (!b->isPressed()) b->onPressed(pos);
                    break;
                } else if (!pressed && b->isPressed()) {
                    b->onReleased();
                }
            }
        }
        if (audio) audio->update();
        update();
        if (menu) menu->update(getCurrentTimeMicros());
        renderer->render();
    }
}

void Game::setTimeMicros(int64_t time) {
    relativeTime -= time - getCurrentTimeMicros();
}

Menu *Game::getMenu(std::string menu) { return menus[menu]; }

Menu *Game::getCurrentMenu() { return menu; }

void Game::resetClock() {
    this->begTime = std::chrono::high_resolution_clock::now();
    this->relativeTime = 0;
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
           (int64_t)relativeTime;
}

void Game::setRenderer(Renderer *renderer) { this->renderer = renderer; }
void Game::setWindow(Window *window) { this->window = window; }

Window *Game::getWindow() { return this->window; }

Renderer *Game::getRenderer() { return renderer; }

TextHandler *Game::getTextHandler() { return renderer->getTextHandler(); }

// TODO replace with render module thingy thing
size_t Game::updateUBO(void *&ubo) { return sizeof(UniformBufferObject); }
void Game::freeUBO(void *&ubo) {}

size_t Game::updateFinalUBO(void *&ubo) { return sizeof(UniformBufferObject); }
void Game::freeFinalUBO(void *&ubo) {}

void Game::addEntity(std::shared_ptr<Entity> &entity) {
    entities.push_back(entity);
}

float prevZ = 0.9999f;

void Game::addGui(std::shared_ptr<Gui> &gui) {
    if (guis.empty())
        prevZ = 0.9999f;
    else
        prevZ -= 0.00001f;
    gui->setZ(prevZ);
    this->guis.push_back(gui);
}

std::vector<std::shared_ptr<Entity>> Game::getEntities() { return entities; }
std::vector<std::shared_ptr<Gui>> Game::getGuis() { return guis; }

void Game::playSound(std::string sound) { this->audio->playSound(sound); }

Game::~Game() {
    resetScene();

    for (auto entry : menus) delete entry.second;

    delete audio;

    delete renderer;
    delete window;

    glfwTerminate();
}
