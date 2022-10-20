#include "MainMenu.hpp"

#include "Synthmania.hpp"

// TODO fix ratios because on fullscreen buttons go past the edge
MainMenu::MainMenu(Game* g) : Menu(g) {
    TextHandler* text = g->getTextHandler();
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

    for (Text t :
         text->createVerticalText("PLAY", "Stupid", 12, glm::vec2({0, -.4}))) {
        ParentedGui* g =
            new ParentedGui(t.character.texture, "PLAY", buttons[0]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.2, .2, .7, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t :
         text->createVerticalText("EDIT", "Stupid", 12, glm::vec2({0, -.4}))) {
        ParentedGui* g =
            new ParentedGui(t.character.texture, "EDIT", buttons[1]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.2, .7, .7, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t : text->createVerticalText("OPTIONS", "Stupid", 12,
                                           glm::vec2({0, -.4}))) {
        ParentedGui* g =
            new ParentedGui(t.character.texture, "OPTIONS", buttons[2]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.7, .7, .2, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t :
         text->createVerticalText("TESTS", "Stupid", 12, glm::vec2({0, -.4}))) {
        ParentedGui* g =
            new ParentedGui(t.character.texture, "TESTS", buttons[3]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.5, .5, .5, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t : text->createVerticalText("SOCIAL", "Stupid", 12,
                                           glm::vec2({0, -.4}))) {
        ParentedGui* g =
            new ParentedGui(t.character.texture, "SOCIAL", buttons[4]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(0, .1, .8, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t : text->createVerticalText("UPDATES", "Stupid", 12,
                                           glm::vec2({0, -.4}))) {
        ParentedGui* g =
            new ParentedGui(t.character.texture, "UPDATES", buttons[5]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.9, 0, .1, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t :
         text->createVerticalText("MAPS", "Stupid", 12, glm::vec2({0, -.4}))) {
        ParentedGui* g =
            new ParentedGui(t.character.texture, "MAPS", buttons[6]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.7, .7, .7, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t :
         text->createVerticalText("EXTRA", "Stupid", 12, glm::vec2({0, -.4}))) {
        ParentedGui* g =
            new ParentedGui(t.character.texture, "EXTRA", buttons[7]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
}

void MainMenu::show() { Menu::show(); }

void MainMenu::onPressed(Button* b) {
    Synthmania* s = dynamic_cast<Synthmania*>(game);
    if (s != NULL) {
        s->playSound("click");
        if (b->getName() == "start") {
            s->resetScene();
            s->loadMenu("song select");
            s->setTimeMicros(-s->getChart().offset);
        } else if (b->getName() == "options") {
            s->resetScene();
            s->loadMenu("options");
        }
    }
}

void MainMenu::update(int64_t time) {}

MainMenu::~MainMenu() {}