#include "MainMenu.hpp"

#include "Synthmania.hpp"

MainMenu::MainMenu(Game* g) : Menu(g) {
    TextHandler* text = g->getTextHandler();
    std::vector<ImageView*> tex = g->getRenderer()->getTextures();
    ImageView *piano = getTextureByName(tex, "piano"),
              *piano_pressed = getTextureByName(tex, "piano-pressed"),
              *black_key = getTextureByName(tex, "black_key"),
              *black_key_pressed = getTextureByName(tex, "black_key-pressed");

    // CDE
    buttons.push_back(std::make_shared<Button>(piano, piano_pressed, "start",
                                               glm::vec2({-1.4, .5f}),
                                               glm::vec2({.4, 1.f})));
    buttons.push_back(std::make_shared<Button>(piano, piano_pressed, "edit",
                                               glm::vec2({-1.f, .5f}),
                                               glm::vec2({.4, 1.f})));
    buttons.push_back(std::make_shared<Button>(piano, piano_pressed, "options",
                                               glm::vec2({-.6, .5f}),
                                               glm::vec2({.4, 1.f})));

    // FGAB
    buttons.push_back(std::make_shared<Button>(piano, piano_pressed, "tests",
                                               glm::vec2({-.2, .5f}),
                                               glm::vec2({.4, 1.f})));
    buttons.push_back(std::make_shared<Button>(piano, piano_pressed, "social",
                                               glm::vec2({.2, .5f}),
                                               glm::vec2({.4, 1.f})));
    buttons.push_back(std::make_shared<Button>(piano, piano_pressed, "updates",
                                               glm::vec2({.6f, .5f}),
                                               glm::vec2({.4, 1.f})));
    buttons.push_back(std::make_shared<Button>(piano, piano_pressed, "maps",
                                               glm::vec2({1.f, .5f}),
                                               glm::vec2({.4, 1.f})));

    buttons.push_back(std::make_shared<Button>(piano, piano_pressed, "extra",
                                               glm::vec2({1.4f, .5f}),
                                               glm::vec2({.4, 1.f})));

    buttons.push_back(std::make_shared<Button>(black_key, black_key_pressed,
                                               "start*", glm::vec2({-1.2, .3f}),
                                               glm::vec2({.2, .6f})));
    buttons.push_back(std::make_shared<Button>(black_key, black_key_pressed,
                                               "edit*", glm::vec2({-.8, .3f}),
                                               glm::vec2({.2, .6f})));

    buttons.push_back(std::make_shared<Button>(black_key, black_key_pressed,
                                               "tests*", glm::vec2({0, .3f}),
                                               glm::vec2({.2, .6f})));
    buttons.push_back(std::make_shared<Button>(black_key, black_key_pressed,
                                               "social*", glm::vec2({.4, .3f}),
                                               glm::vec2({.2, .6f})));
    buttons.push_back(std::make_shared<Button>(black_key, black_key_pressed,
                                               "updates*", glm::vec2({.8, .3f}),
                                               glm::vec2({.2, .6f})));

    for (Text t :
         text->createText("The quick brown fox jumps over the lazy dog",
                          "Stupid", 12, glm::vec2({0, -.5}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "PLAY", buttons[0]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.2, .2, .7, 1));
        g->setNegate(true);
        guis.push_back(g);
    }
    for (Text t :
         text->createVerticalText("PLAY", "Stupid", 12, glm::vec2({0, -.4}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "PLAY", buttons[0]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.2, .2, .7, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t :
         text->createVerticalText("EDIT", "Stupid", 12, glm::vec2({0, -.4}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "EDIT", buttons[1]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.2, .7, .7, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t : text->createVerticalText("OPTIONS", "Stupid", 12,
                                           glm::vec2({0, -.4}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "OPTIONS", buttons[2]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.7, .7, .2, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t :
         text->createVerticalText("TESTS", "Stupid", 12, glm::vec2({0, -.4}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "TESTS", buttons[3]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.5, .5, .5, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t : text->createVerticalText("SOCIAL", "Stupid", 12,
                                           glm::vec2({0, -.4}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "SOCIAL", buttons[4]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(0, .1, .8, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t : text->createVerticalText("UPDATES", "Stupid", 12,
                                           glm::vec2({0, -.4}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "UPDATES", buttons[5]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.9, 0, .1, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t :
         text->createVerticalText("MAPS", "Stupid", 12, glm::vec2({0, -.4}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "MAPS", buttons[6]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.7, .7, .7, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t :
         text->createVerticalText("EXTRA", "Stupid", 12, glm::vec2({0, -.4}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "EXTRA", buttons[7]);
        t.pos.x -= t.size.x / 2;
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
}

void MainMenu::show() { Menu::show(); }

void MainMenu::onPressed(const std::shared_ptr<Button>& b) {
    Synthmania* s = dynamic_cast<Synthmania*>(game);
    if (s != NULL) {
        s->playSound("click");
        if (b->getName() == "start") {
            s->resetScene();
            s->loadMenu("song select");
        } else if (b->getName() == "options") {
            s->resetScene();
            s->loadMenu("options");
        } else if (b->getName() == "extra") {
            if (std::system("open https://github.com/HyperLan-git/Synthmania"))
                ;  // TODO Idk show text with error or popup idec
        }
    }
}

void MainMenu::update(int64_t time) {}

MainMenu::~MainMenu() {}