#include "MainMenu.hpp"

#include "Synthmania.hpp"

MainMenu::MainMenu(Game* g) : Menu(g) {
    std::vector<ImageView*> tex = g->getRenderer()->getTextures();
    ImageView *piano = getTextureByName(tex, "piano"),
              *piano_pressed = getTextureByName(tex, "piano-pressed"),
              *black_key = getTextureByName(tex, "black_key"),
              *black_key_pressed = getTextureByName(tex, "black_key-pressed");

    // CDE
    buttons.push_back(new Button(piano, piano_pressed, "start",
                                 glm::vec2({-1.75, .5f}),
                                 glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(piano, piano_pressed, "edit",
                                 glm::vec2({-1.25, .5f}),
                                 glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(piano, piano_pressed, "options",
                                 glm::vec2({-.75, .5f}), glm::vec2({.5, 1.f})));

    // FGAB
    buttons.push_back(new Button(piano, piano_pressed, "tests",
                                 glm::vec2({-.25, .5f}), glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(piano, piano_pressed, "social",
                                 glm::vec2({.25f, .5f}), glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(piano, piano_pressed, "updates",
                                 glm::vec2({.75f, .5f}), glm::vec2({.5, 1.f})));
    buttons.push_back(new Button(piano, piano_pressed, "maps",
                                 glm::vec2({1.25f, .5f}),
                                 glm::vec2({.5, 1.f})));

    buttons.push_back(new Button(piano, piano_pressed, "extra",
                                 glm::vec2({1.75f, .5f}),
                                 glm::vec2({.5, 1.f})));

    buttons.push_back(new Button(black_key, black_key_pressed, "start*",
                                 glm::vec2({-1.5, .3f}), glm::vec2({.2, .6f})));
    buttons.push_back(new Button(black_key, black_key_pressed, "edit*",
                                 glm::vec2({-1, .3f}), glm::vec2({.2, .6f})));

    buttons.push_back(new Button(black_key, black_key_pressed, "tests*",
                                 glm::vec2({0, .3f}), glm::vec2({.2, .6f})));
    buttons.push_back(new Button(black_key, black_key_pressed, "social*",
                                 glm::vec2({.5, .3f}), glm::vec2({.2, .6f})));
    buttons.push_back(new Button(black_key, black_key_pressed, "updates*",
                                 glm::vec2({1, .3f}), glm::vec2({.2, .6f})));

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
        s->loadMenu("song select");
        s->setTimeMicros(-s->getChart().offset);
    }
}

void MainMenu::update(int64_t time) {}

MainMenu::~MainMenu() {}