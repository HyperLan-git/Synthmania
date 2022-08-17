#include "MainMenu.hpp"

#include "Synthmania.hpp"

MainMenu::MainMenu(Game* g) : Menu(g) {
    std::vector<ImageView*> tex = g->getRenderer()->getTextures();
    buttons.push_back(new Button(
        getTextureByName(tex, "piano"), getTextureByName(tex, "piano-pressed"),
        "start", glm::vec2({-1.75, .5f}), glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(
        getTextureByName(tex, "piano"), getTextureByName(tex, "piano-pressed"),
        "edit", glm::vec2({-1.25, .5f}), glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(
        getTextureByName(tex, "piano"), getTextureByName(tex, "piano-pressed"),
        "options", glm::vec2({-.75, .5f}), glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(
        getTextureByName(tex, "piano"), getTextureByName(tex, "piano-pressed"),
        "tests", glm::vec2({-.25, .5f}), glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(
        getTextureByName(tex, "piano"), getTextureByName(tex, "piano-pressed"),
        "social", glm::vec2({.25f, .5f}), glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(
        getTextureByName(tex, "piano"), getTextureByName(tex, "piano-pressed"),
        "updates", glm::vec2({.75f, .5f}), glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(
        getTextureByName(tex, "piano"), getTextureByName(tex, "piano-pressed"),
        "maps", glm::vec2({1.25f, .5f}), glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(
        getTextureByName(tex, "piano"), getTextureByName(tex, "piano-pressed"),
        "extra", glm::vec2({1.75f, .5f}), glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(getTextureByName(tex, "black_key"),
                                 getTextureByName(tex, "black_key-pressed"),
                                 "start", glm::vec2({-1.5, .3f}),
                                 glm::vec2({.2, .6f})));
    buttons.push_back(new Button(getTextureByName(tex, "black_key"),
                                 getTextureByName(tex, "black_key-pressed"),
                                 "start", glm::vec2({-1, .3f}),
                                 glm::vec2({.2, .6f})));
    buttons.push_back(new Button(getTextureByName(tex, "black_key"),
                                 getTextureByName(tex, "black_key-pressed"),
                                 "start", glm::vec2({0, .3f}),
                                 glm::vec2({.2, .6f})));
    buttons.push_back(new Button(getTextureByName(tex, "black_key"),
                                 getTextureByName(tex, "black_key-pressed"),
                                 "start", glm::vec2({.5, .3f}),
                                 glm::vec2({.2, .6f})));
    buttons.push_back(new Button(getTextureByName(tex, "black_key"),
                                 getTextureByName(tex, "black_key-pressed"),
                                 "start", glm::vec2({1, .3f}),
                                 glm::vec2({.2, .6f})));
    for (Text t : g->getRenderer()->createVerticalText("PLAY", "Stupid", 12,
                                                       glm::vec2({-1.8, .4}))) {
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