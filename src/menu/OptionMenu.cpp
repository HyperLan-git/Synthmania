#include "OptionMenu.hpp"

OptionMenu::OptionMenu(Game* game) : Menu(game) {
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

    std::vector<ImageView*> textures = game->getRenderer()->getTextures();

    Gui* sliderbg = new Gui(getTextureByName(textures, "slider"), "sliderbg");
    sliderbg->setPosition({.3, -.7});
    sliderbg->setSize({.7, .125});
    guis.push_back(sliderbg);
    Slider* volume =
        new Slider(getTextureByName(textures, "slider_button"), "volume", .575);
    volume->setPosition({.3, -.7});
    volume->setSize({.125, .125});
    elements.push_back(volume);
}

void OptionMenu::show() {
    for (Button* b : buttons) game->addGui(b);
    for (Gui* g : guis) game->addGui(g);
    for (GuiElement* e : elements) game->addGui(e);
}

OptionMenu::~OptionMenu() {}