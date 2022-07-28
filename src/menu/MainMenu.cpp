#include "MainMenu.hpp"

#include "Synthmania.hpp"

MainMenu::MainMenu(Game* g) : Menu(g) {
    std::vector<ImageView*> tex = g->getRenderer()->getTextures();
    buttons.push_back(new Button(getTextureByName(tex, "button"),
                                 getTextureByName(tex, "button-pressed"),
                                 "start", glm::vec2({-1, 0}),
                                 glm::vec2({.5, 1.5})));
    for (Text t : g->getRenderer()->createVerticalText("PLAY", "Stupid", 12,
                                                       glm::vec2({-1, -.4}))) {
        Gui* g = new Gui(t.character.texture, "PLAY");
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.2, .2, .7, 1));
        guis.push_back(g);
    }
}

void MainMenu::show() { Menu::show(); }

void MainMenu::onPressed(Button* b) {
    Synthmania* s = dynamic_cast<Synthmania*>(game);
    if (s != NULL) {
        s->playSound("click");
        s->resetScene();
        s->loadSong("resources/songs/ETEA");
        s->setTimeMicros(-s->getChart().offset);
    }
}

void MainMenu::update(int64_t time) {}

MainMenu::~MainMenu() {}