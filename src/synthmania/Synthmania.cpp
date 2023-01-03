#include "Synthmania.hpp"
// TODO fix the white half note being slightly misplaced to the right

Synthmania::Synthmania(std::string skin, std::string config) {
    this->skin = skin;
    this->options = std::make_unique<Options>("resources/default.json", config);
    handler = new MidiHandler();
    audio = new AudioHandler();
    textures = readTextures(std::string(skin).append("/skin.json"));
    for (auto &elem : textures)
        elem.second = std::string(skin).append("/").append(elem.second);
    applyOptions();
    resetAudio();
}

void Synthmania::resetAudio() {
    audio->clearSounds();
    // TODO json file again xd
    audio->addSound("click", new AudioBuffer("resources/sounds/click.wav"));
    audio->addSound("piano", new AudioBuffer("resources/sounds/pianoC4.wav"));
    audio->addSound("kick", new AudioBuffer("resources/sounds/kick.wav"));
    audio->addSound("snare", new AudioBuffer("resources/sounds/snare.wav"));
    audio->addSound("hat", new AudioBuffer("resources/sounds/hat.wav"));
    audio->addSound("ride", new AudioBuffer("resources/sounds/ride.wav"));
    audio->addSound("crash", new AudioBuffer("resources/sounds/crash.wav"));
    audio->addSound("tom-high", new AudioBuffer("resources/sounds/tom1.wav"));
    audio->addSound("tom-mid", new AudioBuffer("resources/sounds/tom2.wav"));
    audio->addSound("tom-low", new AudioBuffer("resources/sounds/tom3.wav"));
}

void Synthmania::init() {
    Game::init();
    this->menus.emplace("main", new MainMenu(this));
    // TODO song folder as option
    this->menus.emplace("song select",
                        new SongSelectMenu(this, "resources/songs"));
    this->menus.emplace("options", new OptionMenu(this));
}

void Synthmania::loadSong(std::string songFolder) {
    setGamemode(new PlayMode(this, songFolder));
}

void Synthmania::setGamemode(Gamemode *gamemode) {
    if (this->gamemode) delete this->gamemode;
    this->gamemode = gamemode;
}

Options *Synthmania::getOptions() { return options.get(); }

void Synthmania::addGui(Gui *gui) {
    Game::addGui(gui);
    if (gamemode) gamemode->onSpawn(gui);
}

void Synthmania::setTimeMicros(int64_t time) {
    Game::setTimeMicros(time);
    if (gamemode) gamemode->onClockAdjust(time);
}

void Synthmania::resetScene() { Game::resetScene(); }

void Synthmania::update() {
    int64_t time_from_start = this->getCurrentTimeMicros();
    std::vector<Gui *> toDestroy;
    for (Gui *g : guis) {
        if (g->update(time_from_start) || g->isDestroyed()) {
            g->setDestroyed();
            toDestroy.push_back(g);
        }
        g->updateGraphics(time_from_start);
    }
    // TODO shared_ptr for guis
    for (Gui *g : toDestroy) {
        for (std::vector<Gui *>::iterator iter = guis.begin();
             iter != guis.end(); iter++)
            if (g == *iter) {
                guis.erase(iter);
                delete g;
                break;
            }
    }
    std::vector<Entity *> toDelete;
    for (Entity *e : entities) {
        if (e->update(time_from_start)) toDelete.push_back(e);
    }
    for (Entity *e : toDelete)
        for (std::vector<Entity *>::iterator iter = entities.begin();
             iter != entities.end(); iter++)
            if (e == *iter) {
                entities.erase(iter);
                delete e;
                break;
            }
    if (gamemode && gamemode->update()) {
        delete gamemode;
        gamemode = NULL;
    }
    std::this_thread::yield();
}
// TODO find a good way to handle fullscreen (link options to window creation)
bool Synthmania::isFullscreen() { return fullscreen; }

void Synthmania::applyOptions() {
    // XXX determine if this is too slow/tedious
    float volume = *options->getValue<float>("audio.volume");
    this->musicVol = volume * *options->getValue<float>("audio.music");
    volume *= *options->getValue<float>("audio.hitsounds");
    int audio = *options->getValue<int>("audio.device");
    if (audio >= 0) {
        std::vector<std::string> devices = getAudioDevices();
        if (audio < devices.size()) {
            std::string device = devices.begin()[audio];
            this->audio->setDevice(device.c_str());
            resetAudio();
        }
    }
    this->audio->setVolume(volume);
    int midi = *options->getValue<int>("midi.device");
    if (midi != -1 && midi != this->handler->getOpenPort() &&
        midi < this->handler->getMidiPorts().size())
        this->handler->openPort(midi);
    // TODO appearance : note names
    this->audioLatency = *options->getValue<long>("gameplay.audio latency");
    this->graphicalLatency =
        *options->getValue<long>("gameplay.graphical latency");
    this->audioLeniency = *options->getValue<long>("gameplay.adjusting delay");
    this->fullscreen = *options->getValue<bool>("appearance.fullscreen");
#ifndef NOVST
    this->bufSize = *options->getValue<int>("plugin.buffer size");
    this->bufAmt = *options->getValue<int>("plugin.buffers");
    this->pluginFolders = std::vector<std::string>({"./plugins"});
    /*    for (auto entry : *options->getValues("plugin.folders"))
        this->pluginFolders.push_back(
            entry.second.get_value<std::string>("plugins"));*/
    this->availablePlugins = readPlugins(this->pluginFolders);
#endif
}

size_t Synthmania::updateUBO(void *&ubo) {
    if (gamemode) return gamemode->updateUBO(ubo);
    return sizeof(UniformBufferObject);
}

void Synthmania::freeUBO(void *&ubo) {
    if (gamemode) gamemode->freeUBO(ubo);
}

size_t Synthmania::updateFinalUBO(void *&ubo) {
    if (gamemode) return gamemode->updateFinalUBO(ubo);
    return sizeof(UniformBufferObject);
}

void Synthmania::freeFinalUBO(void *&ubo) {
    if (gamemode) gamemode->freeUBO(ubo);
}

Synthmania::~Synthmania() {
    if (gamemode) delete gamemode;

    delete handler;
}

std::vector<Gui *> printString(std::string text, TextHandler *textHandler,
                               std::string entityNames, std::string font,
                               double size, glm::vec2 pos, glm::vec4 color) {
    std::vector<Gui *> result;
    int i = 0;
    for (Text t : textHandler->createText(text, font, size, pos)) {
        std::string name = entityNames;
        name.append(std::to_string(i++));
        Gui *gui = new Gui(t.character.texture, name.c_str());
        gui->setColor(color);
        gui->setNegate(1);
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        result.push_back(gui);
    }
    return result;
}

std::vector<Gui *> printShadowedString(std::string text,
                                       TextHandler *textHandler,
                                       std::string entityNames,
                                       std::string font, double size,
                                       glm::vec2 pos, glm::vec4 color) {
    std::vector<Gui *> result, result2;
    glm::vec2 shadowPos = pos;
    shadowPos += glm::vec2({.0005 * size, .0005 * size});
    std::string shadowName = entityNames;
    shadowName.append("shadow_");
    for (Gui *g : printString(text, textHandler, shadowName, font, size,
                              shadowPos, {0, 0, 0, .7})) {
        glm::vec2 sz = g->getSize();
        sz *= 1.05;
        g->setSize(sz);
        result.push_back(g);
    }
    for (Gui *g :
         printString(text, textHandler, entityNames, font, size, pos, color))
        result.push_back(g);
    for (int i = 0; i < result.size() / 2; i++) {
        result2.push_back(result[i]);
        result2.push_back(result[i + result.size() / 2]);
    }

    return result2;
}

std::vector<Gui *> printShakingString(std::string text,
                                      TextHandler *textHandler,
                                      std::string entityNames, std::string font,
                                      double size, glm::vec2 pos, float shake,
                                      glm::vec4 color) {
    std::vector<Gui *> result;
    int i = 0;
    for (Text t : textHandler->createText(text, font, size, pos)) {
        std::string name = entityNames;
        name.append(std::to_string(i++));
        Gui *gui = new Gui(t.character.texture, name.c_str());
        gui->addEffect(new GraphicalEffect(applyShaking,
                                           new float[1]{shake * (float)size}));
        gui->setColor(color);
        gui->setNegate(true);
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        result.push_back(gui);
    }
    return result;
}

std::vector<Gui *> printVerticalString(std::string text,
                                       TextHandler *textHandler,
                                       std::string entityNames,
                                       std::string font, double size,
                                       glm::vec2 pos, glm::vec4 color) {
    std::vector<Gui *> result;
    int i = 0;
    for (Text t : textHandler->createText(text, font, size, pos)) {
        std::string name = entityNames;
        name.append(std::to_string(i++));
        Gui *gui = new Gui(t.character.texture, name.c_str());
        gui->setColor(color);
        gui->setNegate(1);
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        result.push_back(gui);
    }
    return result;
}

float Synthmania::getMusicVolume() { return musicVol; }
int64_t Synthmania::getAudioLeniency() { return audioLeniency; }
int64_t Synthmania::getStartTime() { return startTime; }
MidiHandler *Synthmania::getMidiHandler() { return handler; }
AudioHandler *Synthmania::getAudioHandler() { return audio; }
Gamemode *Synthmania::getGamemode() { return gamemode; }

std::string Synthmania::getPlugin(std::string name) {
    return this->availablePlugins[name];
}