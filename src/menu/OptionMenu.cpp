#include "OptionMenu.hpp"

OptionMenu::OptionMenu(Game* game) : Menu(game) {
    TextHandler* text = game->getTextHandler();
    std::vector<ImageView*> textures = game->getRenderer()->getTextures();
    ImageView *sliderBg = getTextureByName(textures, "slider"),
              *sliderTex = getTextureByName(textures, "slider_button");
    Button* back = new Button(getTextureByName(textures, "button"),
                              getTextureByName(textures, "button-pressed"),
                              "back", {-1.7, .85}, {.6, .3});
    buttons.push_back(back);
    for (Text t : text->createText("Save", "Stupid", 12, glm::vec2({-.1, 0}))) {
        ParentedGui* g =
            new ParentedGui(t.character.texture, "Back_arrow", back);
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    for (Text t :
         text->createText("OPTIONS", "Stupid", 20, glm::vec2({-.5, -.85}))) {
        Gui* g = new Gui(t.character.texture, "Title");
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.7, .7, .2, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Audio", "Stupid", 12, glm::vec2({-1.4, -.7}))) {
        Gui* g = new Gui(t.character.texture, "Audio");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Midi", "Stupid", 12, glm::vec2({-1.4, -.3}))) {
        Gui* g = new Gui(t.character.texture, "Midi");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Gameplay", "Stupid", 12, glm::vec2({-1.4, -.1}))) {
        Gui* g = new Gui(t.character.texture, "Gameplay");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Appearance", "Stupid", 12, glm::vec2({-1.4, .3}))) {
        Gui* g = new Gui(t.character.texture, "Appearance");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Plugin", "Stupid", 12, glm::vec2({-1.4, .7}))) {
        Gui* g = new Gui(t.character.texture, "Plugin");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Volume", "Stupid", 8, glm::vec2({-.2, -.7}))) {
        Gui* g = new Gui(t.character.texture, "Volume");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    Gui* sliderbg = new Gui(sliderBg, "sliderbg");
    sliderbg->setPosition({.8, -.7});
    sliderbg->setSize({1, .15});
    guis.push_back(sliderbg);
    Slider* volume = new Slider(sliderTex, "volume", .85);
    volume->setPosition({.8, -.7});
    volume->setSize({.15, .15});
    elements.push_back(volume);

    for (Text t : text->createText("Music", "Stupid", 8, glm::vec2({0, -.5}))) {
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

    for (Text t :
         text->createText("Synth", "Stupid", 8, glm::vec2({-1.3, -.5}))) {
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

    // TODO put selector guis on top of everything else
    libremidi::midi_in in;
    std::vector<std::string> ports;
    for (int i = 0, N = in.get_port_count(); i < N; i++) {
        std::string name = in.get_port_name(i);
        ports.push_back(name);
    }
    Selector* s = new Selector(getTextureByName(textures, "selector"),
                               getTextureByName(textures, "button"), game,
                               "midi selector", ports, 5, "Stupid", 18);
    s->setPosition({-.6, -.3});
    s->setSize({.7, .15});
    s->recalculatePositions();
    for (Gui* g : s->getGuis()) this->guis.push_back(g);
    this->elements.push_back(s);
    std::vector<std::string> audioDevices = getAudioDevices();
    s = new Selector(getTextureByName(textures, "selector"),
                     getTextureByName(textures, "button"), game,
                     "audio selector", audioDevices, 5, "Stupid", 18);
    s->setPosition({-.6, -.7});
    s->setSize({.7, .15});
    s->recalculatePositions();
    for (Gui* g : s->getGuis()) this->guis.push_back(g);
    this->elements.push_back(s);
    s = new Selector(getTextureByName(textures, "selector"),
                     getTextureByName(textures, "button"), game,
                     "note selector", {"Roman", "Latin"}, 5, "Stupid", 18);
    s->setPosition({-.2, -.1});
    s->setSize({.7, .15});
    s->recalculatePositions();
    for (Gui* g : s->getGuis()) this->guis.push_back(g);
    this->elements.push_back(s);
    wchar_t chars[] = {'I', 0};
    std::wstring str = chars;  //"│";
    audioLatency = new TextArea(getTextureByName(textures, "text_area"),
                                "audio latency", text, "Stupid", 4, 4,
                                (unsigned long)str[0], 12, integerPredicate);
    audioLatency->setSize({1, .175});
    audioLatency->setPosition({.7, -.1});
    for (Gui* g : audioLatency->getGuis()) this->guis.push_back(g);
    audioLatency->recalculateText();
    audioLatency->recalculateCursor();
    this->elements.push_back(audioLatency);
}

void OptionMenu::onPressed(Button* b) {
    Synthmania* s = dynamic_cast<Synthmania*>(game);
    if (s != NULL) {
        s->playSound("click");
        s->resetScene();
        s->loadMenu("main");
    }
}

void OptionMenu::show() {
    for (Button* b : buttons) game->addGui(b);
    for (Gui* g : guis) game->addGui(g);
    for (MenuElement* e : elements) game->addGui(e);
    game->getWindow()->setTextcallback([](GLFWwindow* window, unsigned int c) {
        Game* g = (Game*)(glfwGetWindowUserPointer(window));
        if (g == NULL) return;
        Menu* menu = g->getCurrentMenu();
        if (menu == NULL) return;
        menu->textCallback(window, c);
    });
    game->getWindow()->setKeycallback(
        [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Game* g = (Game*)(glfwGetWindowUserPointer(window));
            if (g == NULL) return;
            Menu* menu = g->getCurrentMenu();
            if (menu == NULL) return;
            menu->keyCallback(window, key, scancode, action, mods);
        });
}

OptionMenu::~OptionMenu() {}