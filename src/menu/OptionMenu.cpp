#include "OptionMenu.hpp"

OptionMenu::OptionMenu(Game* game) : Menu(game) {
    std::vector<ImageView*> textures = game->getRenderer()->getTextures();
    ImageView *sliderBg = getTextureByName(textures, "slider"),
              *sliderTex = getTextureByName(textures, "slider_button");
    for (Text t : game->getRenderer()->createText("OPTIONS", "Stupid", 20,
                                                  glm::vec2({-.5, -.85}))) {
        Gui* g = new Gui(t.character.texture, "Title");
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.7, .7, .2, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t : game->getRenderer()->createText("Audio", "Stupid", 12,
                                                  glm::vec2({-1.4, -.7}))) {
        Gui* g = new Gui(t.character.texture, "Audio");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t : game->getRenderer()->createText("Midi", "Stupid", 12,
                                                  glm::vec2({-1.4, -.3}))) {
        Gui* g = new Gui(t.character.texture, "Midi");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t : game->getRenderer()->createText("Gameplay", "Stupid", 12,
                                                  glm::vec2({-1.4, -.1}))) {
        Gui* g = new Gui(t.character.texture, "Gameplay");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t : game->getRenderer()->createText("Appearance", "Stupid", 12,
                                                  glm::vec2({-1.4, .3}))) {
        Gui* g = new Gui(t.character.texture, "Appearance");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t : game->getRenderer()->createText("Plugin", "Stupid", 12,
                                                  glm::vec2({-1.4, .7}))) {
        Gui* g = new Gui(t.character.texture, "Plugin");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t : game->getRenderer()->createText("Volume", "Stupid", 8,
                                                  glm::vec2({-.5, -.7}))) {
        Gui* g = new Gui(t.character.texture, "Volume");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    Gui* sliderbg = new Gui(sliderBg, "sliderbg");
    sliderbg->setPosition({.5, -.7});
    sliderbg->setSize({1, .15});
    guis.push_back(sliderbg);
    Slider* volume = new Slider(sliderTex, "volume", .85);
    volume->setPosition({.5, -.7});
    volume->setSize({.15, .15});
    elements.push_back(volume);

    for (Text t : game->getRenderer()->createText("Music", "Stupid", 8,
                                                  glm::vec2({0, -.5}))) {
        Gui* g = new Gui(t.character.texture, "Volume");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    sliderbg = new Gui(sliderBg, "sliderbg2");
    sliderbg->setPosition({1, -.5});
    sliderbg->setSize({1, .15});
    guis.push_back(sliderbg);
    volume = new Slider(sliderTex, "music", .85);
    volume->setPosition({1, -.5});
    volume->setSize({.15, .15});
    elements.push_back(volume);

    for (Text t : game->getRenderer()->createText("Synth", "Stupid", 8,
                                                  glm::vec2({-1.3, -.5}))) {
        Gui* g = new Gui(t.character.texture, "Volume");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    sliderbg = new Gui(sliderBg, "sliderbg2");
    sliderbg->setPosition({-.55, -.5});
    sliderbg->setSize({1, .15});
    guis.push_back(sliderbg);
    volume = new Slider(sliderTex, "music", .85);
    volume->setPosition({-.55, -.5});
    volume->setSize({.15, .15});
    elements.push_back(volume);

    libremidi::midi_in in;
    std::vector<std::string> ports;
    for (int i = 0, N = in.get_port_count(); i < N; i++) {
        std::string name = in.get_port_name(i);
        ports.push_back(name);
    }
    Selector* s = new Selector(getTextureByName(textures, "selector"),
                               getTextureByName(textures, "button"), game,
                               "midi selector", ports, 5, "Stupid", 22);
    s->setPosition({-.3, .3});
    s->setSize({.7, .15});
    s->recalculatePositions();
    for (Gui* g : s->getGuis()) this->guis.push_back(g);
    this->elements.push_back(s);
}

void OptionMenu::show() {
    for (Button* b : buttons) game->addGui(b);
    for (Gui* g : guis) game->addGui(g);
    for (MenuElement* e : elements) game->addGui(e);
}

OptionMenu::~OptionMenu() {}