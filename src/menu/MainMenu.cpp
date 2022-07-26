#include "MainMenu.hpp"

#include "Synthmania.hpp"

MainMenu::MainMenu(Game* g) : Menu(g) {
    std::vector<ImageView*> tex = g->getRenderer()->getTextures();
    buttons.push_back(new Button(getTextureByName(tex, "button"),
                                 getTextureByName(tex, "button-pressed"),
                                 "start", glm::vec2({0, 0}),
                                 glm::vec2({2, 1})));
}

void MainMenu::show() { Menu::show(); }

void MainMenu::onPressed(Button* b) {
    Synthmania* s = dynamic_cast<Synthmania*>(game);
    if (s != NULL) {
        s->resetScene();
        s->loadSong("resources/songs/ETEA");
    }
}

void MainMenu::update(int64_t time) {}

MainMenu::~MainMenu() {}