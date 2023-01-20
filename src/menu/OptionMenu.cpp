#include "OptionMenu.hpp"

OptionMenu::OptionMenu(Game* game) : Menu(game) {
    options = dynamic_cast<Synthmania*>(game)->getOptions();
    TextHandler* text = game->getTextHandler();
    std::vector<ImageView*> textures = game->getRenderer()->getTextures();
    ImageView *sliderBg = getTextureByName(textures, "slider"),
              *sliderTex = getTextureByName(textures, "slider_button");
    std::shared_ptr<Button> back = std::make_shared<Button>(
        getTextureByName(textures, "button"),
        getTextureByName(textures, "button-pressed"), "back",
        glm::vec2({-1.5, .85}), glm::vec2({.6, .3}));
    buttons.push_back(back);
    for (Text t : text->createText("Save", "Stupid", 12, glm::vec2({-.1, 0}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "Back_arrow", back);
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    for (Text t :
         text->createText("OPTIONS", "Stupid", 20, glm::vec2({-.5, -.85}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Title");
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(.7, .7, .2, 1));
        g->setNegate(true);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Audio", "Stupid", 12, glm::vec2({-1.4, -.7}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Audio");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Midi", "Stupid", 12, glm::vec2({-1.4, -.3}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Midi");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Gameplay", "Stupid", 12, glm::vec2({-1.4, -.1}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Gameplay");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Appearance", "Stupid", 12, glm::vec2({-1.4, .3}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Appearance");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Plugin", "Stupid", 12, glm::vec2({-1.4, .6}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Plugin");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    for (Text t :
         text->createText("Volume", "Stupid", 8, glm::vec2({-.2, -.7}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Volume");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    std::shared_ptr<Gui> sliderbg = std::make_shared<Gui>(sliderBg, "sliderbg");
    sliderbg->setPosition({.8, -.7});
    sliderbg->setSize({1, .15});
    guis.push_back(sliderbg);
    volume = std::make_shared<Slider>(sliderTex, "volume", .85);
    volume->setPosition({.8, -.7});
    volume->setSize({.15, .15});
    volume->setValue(*(options->getValue<float>("audio.volume")));
    elements.push_back(volume);

    for (Text t : text->createText("Music", "Stupid", 8, glm::vec2({0, -.5}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Volume");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    sliderbg = std::make_shared<Gui>(sliderBg, "sliderbg2");
    sliderbg->setPosition({1, -.5});
    sliderbg->setSize({1, .15});
    guis.push_back(sliderbg);
    music = std::make_shared<Slider>(sliderTex, "music", .85);
    music->setPosition({1, -.5});
    music->setSize({.15, .15});
    music->setValue(*(options->getValue<float>("audio.music")));
    elements.push_back(music);

    for (Text t :
         text->createText("Synth", "Stupid", 8, glm::vec2({-1.3, -.5}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "synth");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    sliderbg = std::make_shared<Gui>(sliderBg, "sliderbg2");
    sliderbg->setPosition({-.55, -.5});
    sliderbg->setSize({1, .15});
    guis.push_back(sliderbg);
    synth = std::make_shared<Slider>(sliderTex, "synth", .85);
    synth->setPosition({-.55, -.5});
    synth->setSize({.15, .15});
    synth->setValue(*(options->getValue<float>("audio.hitsounds")));
    elements.push_back(synth);

    // TODO put selector guis on top of everything else
    libremidi::midi_in in;
    std::vector<std::string> ports;
    for (int i = 0, N = in.get_port_count(); i < N; i++) {
        std::string name = in.get_port_name(i);
        ports.push_back(name);
    }
    midiDevice =
        std::make_shared<Selector>(getTextureByName(textures, "selector"),
                                   getTextureByName(textures, "button"), game,
                                   "midi selector", ports, 5, "Stupid", 18);
    midiDevice->setPosition({-.6, -.3});
    midiDevice->setSize({.7, .15});
    midiDevice->select(*(options->getValue<int>("midi.device")));
    midiDevice->recalculatePositions();
    for (std::shared_ptr<Gui>& g : midiDevice->getGuis())
        this->guis.push_back(g);
    this->elements.push_back(midiDevice);
    std::vector<std::string> audioDevices = getAudioDevices();
    audioDevice = std::make_shared<Selector>(
        getTextureByName(textures, "selector"),
        getTextureByName(textures, "button"), game, "audio selector",
        audioDevices, 5, "Stupid", 18);
    audioDevice->setPosition({-.6, -.7});
    audioDevice->setSize({.7, .15});
    audioDevice->select(*(options->getValue<int>("audio.device")));
    audioDevice->recalculatePositions();
    for (std::shared_ptr<Gui> g : audioDevice->getGuis())
        this->guis.push_back(g);
    this->elements.push_back(audioDevice);
    notation = std::make_shared<Selector>(
        getTextureByName(textures, "selector"),
        getTextureByName(textures, "button"), game, "note selector",
        std::vector<std::string>({"Roman", "Latin"}), 5, "Stupid", 18);
    notation->setPosition({-.2, -.1});
    notation->setSize({.7, .15});
    notation->select(*(options->getValue<int>("gameplay.notes")));
    notation->recalculatePositions();
    for (std::shared_ptr<Gui> g : notation->getGuis()) this->guis.push_back(g);
    this->elements.push_back(notation);
    for (Text t : text->createText("Audio latency", "Stupid", 6,
                                   glm::vec2({.17, -.1}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Audio latency");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    wchar_t c = L'I';  //"│";
    audioLatency = std::make_shared<TextArea>(
        getTextureByName(textures, "text_area"), "audio latency", text,
        "Stupid", 4, 4, c, 8, integerPredicate);
    audioLatency->setSize({.4, .125});
    audioLatency->setPosition({.8, -.1});
    std::string latency =
        std::to_string(*(options->getValue<int>("gameplay.audio latency")));
    audioLatency->setTextStr(latency);
    for (std::shared_ptr<Gui>& g : audioLatency->getGuis())
        this->guis.push_back(g);
    audioLatency->recalculateText();
    audioLatency->recalculateCursor();
    this->elements.push_back(audioLatency);
    for (Text t : text->createText("Video latency", "Stupid", 6,
                                   glm::vec2({.17, -.3}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Video latency");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    graphicLatency = std::make_shared<TextArea>(
        getTextureByName(textures, "text_area"), "video latency", text,
        "Stupid", 4, 4, c, 8, integerPredicate);
    graphicLatency->setSize({.4, .125});
    graphicLatency->setPosition({.8, -.3});
    latency =
        std::to_string(*(options->getValue<int>("gameplay.graphical latency")));
    graphicLatency->setTextStr(latency);
    for (std::shared_ptr<Gui> g : graphicLatency->getGuis())
        this->guis.push_back(g);
    graphicLatency->recalculateText();
    graphicLatency->recalculateCursor();
    this->elements.push_back(graphicLatency);
    for (Text t : text->createText("Audio leniency", "Stupid", 6,
                                   glm::vec2({-1.4, .1}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Audio leniency");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    audioLeniency = std::make_shared<TextArea>(
        getTextureByName(textures, "text_area"), "audio leniency", text,
        "Stupid", 5, 5, c, 8, integerPredicate);
    audioLeniency->setSize({.4, .125});
    audioLeniency->setPosition({-.7, .1});
    audioLeniency->setText(
        std::to_wstring(*(options->getValue<int>("gameplay.adjusting delay"))));
    for (std::shared_ptr<Gui> g : audioLeniency->getGuis())
        this->guis.push_back(g);
    audioLeniency->recalculateText();
    audioLeniency->recalculateCursor();
    this->elements.push_back(audioLeniency);
    for (Text t :
         text->createText("Fullscreen", "Stupid", 6, glm::vec2({-.6, .3}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "Fullscreen");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    fullscreen = std::make_shared<Checkbox>(
        getTextureByName(textures, "checkbox"),
        getTextureByName(textures, "checkbox-pressed"), "fullscreen");
    fullscreen->setPosition({-.2, .3});
    fullscreen->setSize({.15, .15});
    fullscreen->check(*(options->getValue<bool>("appearance.fullscreen")));
    this->elements.push_back(fullscreen);

    for (Text t : text->createText("Skin", "Stupid", 6, glm::vec2({0, .3}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "skin");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    // TODO make skin system
    skin = std::make_shared<Selector>(
        getTextureByName(textures, "selector"),
        getTextureByName(textures, "button"), game, "note selector",
        std::vector<std::string>({"default"}), 5, "Stupid", 18);
    skin->setPosition({.5, .3});
    skin->setSize({.7, .15});
    skin->select(*(options->getValue<int>("appearance.skin")));
    skin->recalculatePositions();
    for (std::shared_ptr<Gui> g : skin->getGuis()) this->guis.push_back(g);
    this->elements.push_back(skin);
    for (Text t :
         text->createText("Buffer size", "Stupid", 6, glm::vec2({-.9, .6}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "buf size");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    for (Text t :
         text->createText("Audio buffers", "Stupid", 6, glm::vec2({0, .6}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "audio buffers");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    bufSize = std::make_shared<TextArea>(
        getTextureByName(textures, "text_area"), "buffer size", text, "Stupid",
        4, 4, c, 8, integerPredicate);
    bufSize->setSize({.4, .125});
    bufSize->setPosition({-.3, .6});
    bufSize->setText(
        std::to_wstring(*(options->getValue<int>("plugin.buffer size"))));
    for (std::shared_ptr<Gui> g : bufSize->getGuis()) this->guis.push_back(g);
    bufSize->recalculateText();
    bufSize->recalculateCursor();
    this->elements.push_back(bufSize);
    bufAmt = std::make_shared<TextArea>(getTextureByName(textures, "text_area"),
                                        "buffer amt", text, "Stupid", 2, 2, c,
                                        8, integerPredicate);
    bufAmt->setSize({.4, .125});
    bufAmt->setPosition({.7, .6});
    bufAmt->setText(
        std::to_wstring(*(options->getValue<int>("plugin.buffers"))));
    for (std::shared_ptr<Gui> g : bufAmt->getGuis()) this->guis.push_back(g);
    bufAmt->recalculateText();
    bufAmt->recalculateCursor();
    this->elements.push_back(bufAmt);
    for (Text t :
         text->createText("Plugin folders", "Stupid", 6, glm::vec2({-1, .8}))) {
        std::shared_ptr<Gui> g =
            std::make_shared<Gui>(t.character.texture, "plugin folders");
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    pluginFolders = std::make_shared<TextArea>(
        getTextureByName(textures, "text_area"), "plugin folders", text,
        "Stupid", 23, 256, c, 8, truePredicate);
    pluginFolders->setSize({1.4, .125});
    pluginFolders->setPosition({.3, .8});
    std::string folders = "";

    std::optional<tree> values = options->getValues("plugin.folders");
    if (values)
        for (auto entry : *values)
            folders += entry.second.get_value<std::string>("") + ';';
    pluginFolders->setTextStr(folders);
    for (std::shared_ptr<Gui> g : pluginFolders->getGuis())
        this->guis.push_back(g);
    pluginFolders->recalculateText();
    pluginFolders->recalculateCursor();
    this->elements.push_back(pluginFolders);
}

void OptionMenu::onPressed(const std::shared_ptr<Button>& b) {
    Synthmania* s = dynamic_cast<Synthmania*>(game);
    if (s != NULL) {
        save();
        s->applyOptions();
        s->resetAudio();
        s->playSound("click");
        s->resetScene();
        s->loadMenu("main");
    }
}

void OptionMenu::save() {
    options->setValue<float>("audio.volume", this->volume->getValue());
    options->setValue<float>("audio.music", this->music->getValue());
    options->setValue<float>("audio.hitsounds", this->synth->getValue());
    options->setValue<int>("audio.device", this->audioDevice->getSelected());
    options->setValue<int>("midi.device", this->midiDevice->getSelected());
    options->setValue<int>("gameplay.notes", this->notation->getSelected());
    options->setValue<int>("gameplay.audio latency",
                           std::stoi(this->audioLatency->getText()));
    options->setValue<int>("gameplay.graphical latency",
                           std::stoi(this->graphicLatency->getText()));
    options->setValue<int>("gameplay.adjusting delay",
                           std::stoi(this->audioLeniency->getText()));
    options->setValue<bool>("appearance.fullscreen",
                            this->fullscreen->isChecked());
    // options->setValue<std::string>("appearance.skin", this->skin->getText());
    options->setValue<int>("plugin.buffer size",
                           std::stoi(this->bufSize->getText()));
    options->setValue<int>("plugin.buffers",
                           std::stoi(this->bufAmt->getText()));
    tree t, children;
    std::wstring wstr = this->pluginFolders->getText();
    std::stringstream stream(std::string(wstr.begin(), wstr.end()));
    std::string tok;
    while (std::getline(stream, tok, ';'))
        if (tok.size() > 0) {
            tree temp;
            temp.put("", tok);
            t.push_back(std::make_pair("", temp));
        }
    options->setValues("plugin.folders", t);
    std::ofstream stream2(options->getPath(), std::ios::out);
    stream2 << options->save();
}

void OptionMenu::show() {
    for (std::shared_ptr<Button>& b : buttons) game->addTGui(b);
    for (std::shared_ptr<Gui>& g : guis) game->addGui(g);
    for (std::shared_ptr<MenuElement>& e : elements) game->addTGui(e);
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