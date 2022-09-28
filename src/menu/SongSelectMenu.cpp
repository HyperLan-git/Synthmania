#include "SongSelectMenu.hpp"

SongSelectMenu::SongSelectMenu(Game *g, std::string folder) : Menu(g) {
    std::vector<ImageView *> tex = g->getRenderer()->getTextures();
    Button *back = new Button(getTextureByName(tex, "button"),
                              getTextureByName(tex, "button-pressed"), "back",
                              {-1.7, .85}, {.6, .3});
    buttons.push_back(back);
    for (Text t : g->getTextHandler()->createText("Back", "Stupid", 12,
                                                  glm::vec2({-.1, 0}))) {
        ParentedGui *g =
            new ParentedGui(t.character.texture, "Back_arrow", back);
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
            songs.emplace_back(iter.path().filename().c_str());
            std::string &name = songs[songs.size() - 1];
            Button *but = new Button(
                getTextureByName(g->getRenderer()->getTextures(), "button"),
                getTextureByName(g->getRenderer()->getTextures(),
                                 "button-pressed"),
                name.c_str(), {0, pos}, {1, .5});
            this->buttons.push_back(but);
            for (Text t : g->getTextHandler()->createText(
                     name, "Stupid", 10, glm::vec2({-.45, 0}))) {
                ParentedGui *g = new ParentedGui(t.character.texture,
                                                 "Song name " + name, but);
                g->setPosition(t.pos);
                g->setSize(t.size);
                guis.push_back(g);
            }
            pos -= 0.5;
        }
    }
}

void SongSelectMenu::show() {
    game->getWindow()->setMouseWheelcallback([](GLFWwindow *window,
                                                double xoffset,
                                                double yoffset) {
        Game *g = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
        std::vector<Button *> buttons = g->getMenu("song select")->getButtons();
        Button *first = buttons[1], *last = buttons[buttons.size() - 1];
        if (last->getPosition().y + yoffset / 5. <= -1) return;
        if (first->getPosition().y + yoffset / 5. >= 1) return;

        for (auto iter = (++buttons.begin()); iter != buttons.end(); iter++) {
            Button *b = *iter;
            glm::vec3 pos = b->getPosition();
            pos.y += yoffset / 5.;
            b->setPosition(pos);
        }
    });

    Menu::show();
}

void SongSelectMenu::onPressed(Button *b) {
    Synthmania *s = dynamic_cast<Synthmania *>(game);
    if (s != NULL) {
        s->playSound("click");
        s->resetScene();
        if (b->getName() == "back") {
            s->loadMenu("main");
        } else {
            s->loadSong(folder + "/" + b->getName());
            s->setTimeMicros(-s->getChart().offset);
        }
    }
}

void SongSelectMenu::update(int64_t time) {}

SongSelectMenu::~SongSelectMenu() {}