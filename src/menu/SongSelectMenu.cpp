#include "SongSelectMenu.hpp"

SongSelectMenu::SongSelectMenu(Game *g, std::string folder) : Menu(g) {
    Texture button("button"), button_pressed("button-pressed");
    std::shared_ptr<Button> back =
        std::make_shared<Button>(button, button_pressed, "back",
                                 glm::vec2({-1.5, .85}), glm::vec2({.6, .3}));
    buttons.push_back(back);
    for (Text t : g->getTextHandler()->createText("Back", "Stupid", 12,
                                                  glm::vec2({-.1, 0}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "Back_arrow", back);
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    this->folder = folder;
    if (!std::filesystem::exists(folder)) return;
    // Default constructor gives end iterator
    std::filesystem::directory_iterator end;
    float pos = 1;
    for (const auto &iter : std::filesystem::directory_iterator(folder)) {
        if (iter.is_directory()) {
            std::filesystem::path p = iter.path().filename();
            std::string filename = p.string();
            songs.emplace_back(filename);
            std::string &name = songs[songs.size() - 1];
            std::shared_ptr<Button> but = std::make_shared<Button>(
                button, button_pressed, name.c_str(), glm::vec2({0, pos}),
                glm::vec2({1, .25}));
            this->buttons.push_back(but);
            for (Text t : g->getTextHandler()->createText(
                     name, "Stupid", 10, glm::vec2({-.45, 0}))) {
                std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
                    t.character.texture, "Song name " + name, but);
                g->setPosition(t.pos);
                g->setSize(t.size);
                guis.push_back(g);
            }
            pos -= 0.25;
        }
    }
}

void SongSelectMenu::show() {
    game->getWindow()->setMouseWheelcallback([](GLFWwindow *window,
                                                double xoffset,
                                                double yoffset) {
        Game *g = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
        std::vector<std::shared_ptr<Button>> buttons =
            g->getMenu("song select")->getButtons();
        std::shared_ptr<Button> first = buttons[1],
                                last = buttons[buttons.size() - 1];
        if (last->getPosition().y + yoffset / 5. <= -1) return;
        if (first->getPosition().y + yoffset / 5. >= 1) return;

        for (auto iter = (++buttons.begin()); iter != buttons.end(); iter++) {
            std::shared_ptr<Button> b = *iter;
            glm::vec3 pos = b->getPosition();
            pos.y += yoffset / 5.;
            b->setPosition(pos);
        }
    });

    Menu::show();
}

void SongSelectMenu::onPressed(const std::shared_ptr<Button> &b) {
    Synthmania *s = dynamic_cast<Synthmania *>(game);
    if (s != NULL) {
        s->playSound("click");
        s->resetScene();
        if (b->getName() == "back") {
            s->loadMenu("main");
        } else {
            s->loadSong(folder + "/" + b->getName());
        }
    }
}

void SongSelectMenu::update(int64_t time) {}

SongSelectMenu::~SongSelectMenu() {}