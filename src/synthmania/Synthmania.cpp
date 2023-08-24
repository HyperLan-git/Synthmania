#include "Synthmania.hpp"
// TODO fix the white half note being slightly misplaced to the right

Synthmania::Synthmania(std::string skin, std::string config) {
    this->skin = skin;
    this->options = std::make_unique<Options>("resources/default.json", config);
    handler = std::make_unique<MidiHandler>();
    audio = std::make_unique<AudioHandler>();
    textures = readTextures(std::string(skin).append("/skin.json"));
    for (auto &elem : textures)
        elem.second = std::string(skin).append("/").append(elem.second);
    applyOptions();
    resetAudio();
}

void Synthmania::resetAudio() {
    audio->clearSounds();
    // TODO json file again xd
    audio->addSound("click",
                    std::move(AudioBuffer("resources/sounds/click.wav")));
    // TODO handle soundfonts?
    audio->addSound("piano", AudioBuffer("resources/sounds/painoC4.wav"));
    audio->addSound("kick", AudioBuffer("resources/sounds/kick.wav"));
    audio->addSound("snare", AudioBuffer("resources/sounds/snare.wav"));
    audio->addSound("hat", AudioBuffer("resources/sounds/hat.wav"));
    audio->addSound("ride", AudioBuffer("resources/sounds/ride.wav"));
    audio->addSound("crash", AudioBuffer("resources/sounds/crash.wav"));
    audio->addSound("tom-high", AudioBuffer("resources/sounds/tom1.wav"));
    audio->addSound("tom-mid", AudioBuffer("resources/sounds/tom2.wav"));
    audio->addSound("tom-low", AudioBuffer("resources/sounds/tom3.wav"));
}

void Synthmania::init() {
    Game::init();
    this->menus.emplace("main", std::make_shared<MainMenu>(*this));
    // TODO song folder as option
    this->menus.emplace("song select", std::make_shared<SongSelectMenu>(
                                           *this, "resources/songs"));
    this->menus.emplace("options", std::make_shared<OptionMenu>(*this));
    this->menus.emplace("score", std::make_shared<ScoreScreen>(*this));
}

void Synthmania::loadSong(std::string songFolder) {
    setGamemode(std::make_shared<PlayMode>(*this, songFolder));
}

void Synthmania::setGamemode(std::shared_ptr<Gamemode> gamemode) {
    this->gamemode = gamemode;
}

Options &Synthmania::getOptions() { return *options; }

void Synthmania::addGui(std::shared_ptr<Gui> &gui) {
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

    this->guis.erase(std::remove_if(guis.begin(), guis.end(),
                                    [=](std::shared_ptr<Gui> &g) {
                                        return g->update(time_from_start) ||
                                               g->isDestroyed();
                                    }),
                     this->guis.end());
    // TODO send update graphics elsewhere
    for (std::shared_ptr<Gui> &g : guis) g->updateGraphics(time_from_start);
    if (!this->entities.empty())
        this->entities.erase(
            std::remove_if(entities.begin(), entities.end(),
                           [=](std::shared_ptr<Entity> &e) {
                               return e->update(time_from_start);
                           }),
            this->entities.end());
    if (gamemode && gamemode->update()) gamemode.reset();

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
#ifdef VST
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
    if (gamemode) gamemode->freeFinalUBO(ubo);
}

Synthmania::~Synthmania() {
    gamemode.reset();

    handler.reset();
}

float Synthmania::getMusicVolume() { return musicVol; }
int64_t Synthmania::getAudioLeniency() { return audioLeniency; }
int64_t Synthmania::getStartTime() { return startTime; }
MidiHandler &Synthmania::getMidiHandler() { return *handler; }
AudioHandler &Synthmania::getAudioHandler() { return *audio; }
Gamemode &Synthmania::getGamemode() { return *gamemode; }

Score Synthmania::getLastScore() { return score; }
void Synthmania::setLastScore(Score score) { this->score = score; }

void Synthmania::setReplay(uint64_t start, std::vector<Message> replay) {
    this->replayStart = start;
    this->replay = replay;
}
std::vector<Message> Synthmania::getReplay() { return replay; }
uint64_t Synthmania::getReplayStart() { return replayStart; }

#ifdef VST
std::string Synthmania::getPlugin(std::string name) {
    return this->availablePlugins[name];
}
#endif