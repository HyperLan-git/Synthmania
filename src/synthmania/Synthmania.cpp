#include "Synthmania.hpp"
// TODO fix the white half note being slightly misplaced to the right

Synthmania::Synthmania(std::string skin, std::string config) {
    this->skin = skin;
    this->options = std::make_unique<Options>("resources/default.json", config);
    handler = new MidiHandler();
    textures = readTextures(std::string(skin).append("/skin.json"));
    for (auto &elem : textures)
        elem.second = std::string(skin).append("/").append(elem.second);
    audio = new AudioHandler();
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
    if (music != NULL) {
        // TODO recreate current music
    }
#ifndef NOVST
    if (plugin != NULL) delete plugin;
#endif
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
    window->setKeycallback(Synthmania::keyCallback);
    this->songFolder = songFolder;
    std::string json = songFolder;
    json.append("/sdata.json");
    chart = readChart(json.c_str());
    diff = chart.diffs[0];
    this->startTime = chart.offset;
    // TODO could be in a function
    if (chart.animation.compare("None") != 0) {
        void *shared = loadShared(songFolder + "/" + chart.animation);
        char *e = NULL;  // TODO dlerror();
        if (e != NULL) {
            std::cerr << "Error while loading anim ! " << e << "\n";
        } else {
            ChartHandler *(*f)(Synthmania *) =
                (ChartHandler * (*)(Synthmania *))
                    getFunction(shared, "getChartHandler");
            char *e = NULL;  // TODO dlerror();
            if (e != NULL)
                std::cerr << "Error while loading anim ! " << e << "\n";
            else
                this->mod = f(this);  // Shit
        }
        std::string v = this->mod->getVertShaderCode(),
                    g = this->mod->getGeomShaderCode(),
                    f = this->mod->getFragShaderCode(),
                    v_f = this->mod->getFinalVertShaderCode(),
                    g_f = this->mod->getFinalGeomShaderCode(),
                    f_f = this->mod->getFinalFragShaderCode();
        VkDeviceSize UBOSize = this->mod->getUBOSize(),
                     fUBOSize = this->mod->getFinalUBOSize();
        if (!v.empty() || !g.empty() || !f.empty())
            renderer->loadGuiShaders(v, g, f, UBOSize);
        if (!v_f.empty() || !g_f.empty() || !f_f.empty())
            renderer->loadFinalShaders(v_f, g_f, f_f, fUBOSize);
    }
    std::string path = songFolder;
    path.append("/");
    path.append(diff.midi);
    partition = handler->readMidi(path.c_str());
    this->drum = partition.drumming;
    Model *model =
        new Model("resources/models/room.obj", renderer->getPhysicalDevice(),
                  renderer->getDevice());
    std::vector<ImageView *> textures = renderer->getTextures();
    Entity *la_creatura76 =
        new Entity(model, getTextureByName(textures, "room"), "Bob");
    renderer->addModel(model);
    entities.push_back(la_creatura76);
    Key k = drum ? Key::DRUM : Key::SOL;
    std::string keyName = (drum) ? "drum_key" : "sol_key";
    Gui *part = new Gui(getTextureByName(textures, "partition"), "partition"),
        *bg = new Gui(getTextureByName(textures, "background"), "bg"),
        *precision =
            new Gui(getTextureByName(textures, "precision"), "precision"),
        *key = new Gui(getTextureByName(textures, keyName), "key");
    switch (k) {
        case Key::SOL:
            key->setPosition({-1.6f, 0.1f});
            key->setSize({0.8f, 0.8f});
            break;
        case Key::DRUM:
            key->setPosition({-1.6f, 0});
            key->setSize({0.43f, 0.43f});
            break;
        case Key::FA:
            key->setPosition({-1.6f, 0.1f});
            key->setSize({0.8f, 0.8f});
            break;
    }
    addGui(bg);
    addGui(part);
    addGui(key);
    bg->setSize({5, 30});
    part->setSize({5, 1});
    precision->setSize({1.5f, 0.5f});
    precision->setPosition({0, 0.9f});
    Judgement *bar = new Judgement("judgement", textures, partition);
    line = bar;
    bar->setPosition({-1.3f, bar->getPosition().y});
    bar->setSize({0.25f, 1.f});
    addGui(bar);
    std::vector<Gui *> tempNotes;
    for (MidiNote note : partition.notes) {
        std::string name = "Note_";
        std::string hash = std::to_string(std::hash<MidiNote>()(note));
        name += hash;
        double totalDuration = note.length / (long double)partition.MPQ / 4.;
        std::vector<double> cutDown = splitDuration(totalDuration);
        short firstDots = 0;
        for (int i = 1; i < cutDown.size(); i++)
            if (cutDown.size() > 1 && cutDown[i] == cutDown[i - 1] / 2.)
                firstDots = i;
        for (int i = 0; i < firstDots; i++) {
            cutDown[1] += cutDown[0];
            cutDown.erase(cutDown.begin());
        }
        Note *n =
            new Note(name.c_str(), note.timestamp, note.note, totalDuration,
                     cutDown[0], partition.MPQ, textures, k);
        notes.push_back(n);
        int diff =
            getDifferenceFromC4(transposePitch(k, note.note)) + getOffset(k);
        if (diff <= 0 || diff >= 12) {
            bool up = diff >= 12;
            for (int i = up ? 12 : 0; (up && i <= diff) || (!up && i >= diff);
                 up ? i += 2 : i -= 2) {
                std::string barName = "Bar_";
                barName.append(hash);
                PartitionNotation *readability = new PartitionNotation(
                    barName.c_str(), note.timestamp, (0.5 - 0.083 * i),
                    getTextureByName(textures, "bar"));
                readability->setPosition({0, readability->getPosition().y});
                readability->setSize({0.25, 0.15});
                addGui(readability);
            }
        }

        double p = .2;
        for (int i = 0; i < firstDots; i++) {
            ParentedGui *dot = new ParentedGui(
                getTextureByName(textures, "dot"), name.c_str(), n);
            dot->setSize({.05, .05});
            dot->setPosition({p, 0});
            tempNotes.push_back(dot);
            p += .2;
        }

        if (k != Key::DRUM && !isFromCMajor(note.note)) {
            std::string sharpName = "Sharp_";
            sharpName.append(hash);
            ParentedGui *sharp = new ParentedGui(
                getTextureByName(textures, "sharp"), sharpName.c_str(), n);
            sharp->setPosition({-0.25, 0});
            sharp->setSize({0.25, 0.25});
            tempNotes.push_back(sharp);
        }
        uint64_t last = note.timestamp;
        uint64_t t = note.timestamp + cutDown[0] * partition.MPQ * 4;
        for (int i = 1; i < cutDown.size(); i++) {
            std::string name2 = name;
            name2.append("_");
            name2.append(std::to_string(i));
            double d = cutDown[i];
            ParentedGui *p = new ParentedGui(
                getTextureForNote(textures, note.note, d, k), name2.c_str(), n);
            glm::vec2 temp = getSizeAndLocForNote(d, k, note.note);
            p->setPosition({(t - note.timestamp) / 300000.f, 0});
            p->addEffect(
                new GraphicalEffect(applyOffset, new float[]{0, temp.x}));
            p->setSize({temp.y, temp.y});
            tempNotes.push_back(p);
            ParentedGui *arc = new ParentedGui(
                getTextureByName(textures, "arc"), name2.c_str(), n);
            arc->setPosition(
                {(((t + last) / 2) - note.timestamp) / 300000.f, 0});
            arc->addEffect(new GraphicalEffect(applyOffset,
                                               new float[]{0, temp.x - .15f}));
            arc->setSize({(t - last) / 350000.f, .15f});
            tempNotes.push_back(arc);
            last = t;
            t += d * partition.MPQ * 4;
        }
    }
    std::vector<Gui *> tmp;
    for (Note *note : notes) tmp.push_back(note);
    if (tmp.size() > 0) sortGuis(tmp, cmpGuisByTexture);
    if (tempNotes.size() > 0) sortGuis(tempNotes, cmpGuisByTexture);
    for (Gui *note : tmp) addGui(note);
    for (Gui *gui : tempNotes) addGui(gui);
    music = NULL;
    if (chart.audio.compare("None") != 0) {
        std::string wav = songFolder;
        wav.append("/");
        wav.append(chart.audio);
        AudioBuffer *buffer = new AudioBuffer(wav.c_str());

        audio->addSound("song", buffer);
        music = audio->playSound("song");
        music->setGain(musicVol);
        music->setDestroyOnFinished(false);
    }
    std::string pdata = "None";
    if (chart.plugindata.compare("None") != 0) {
        pdata = songFolder;
        pdata.append("/");
        pdata.append(chart.plugindata);
    }
#ifndef NOVST
    if (this->availablePlugins.find(chart.plugin) !=
        this->availablePlugins.end())
        plugin = new AudioPluginHandler(this->availablePlugins[chart.plugin],
                                        audio, pdata);
    else if (chart.plugin.compare("None") != 0)
        std::cout << "Could not find plugin : " << chart.plugin << std::endl;
#endif
    std::string text = chart.name;
    text.append(" by ");
    text.append(chart.artist);

    for (Gui *g :
         printShadowedString(text, renderer->getTextHandler(), "title_",
                             "Stupid", 11, {-1.75, -.9}, {.2, .2, 1, 1}))
        addGui(g);

    /*for (Gui *g : printShakingString("ANGERY", renderer, "scary_", "Stupid",
       22, {0, 0}, .001, {1, 0, 0, 1})) addGui(g);*/
    // Needs to be above everything else
    addGui(precision);
    begTime = std::chrono::high_resolution_clock::now();
}

void Synthmania::endSong() {
    if (music != NULL) {
        audio->removeSound(this->music);
        this->music = NULL;
    }
    this->autoPlay = false;
    this->drum = false;
    if (this->mod != NULL) {
        delete this->mod;
        this->mod = NULL;
        renderer->loadGuiShaders("bin/gui.vert.spv", "bin/def.geom.spv",
                                 "bin/def.frag.spv",
                                 sizeof(UniformBufferObject));
        renderer->loadFinalShaders("bin/pass.vert.spv", "bin/pass.geom.spv",
                                   "bin/pass.frag.spv",
                                   sizeof(UniformBufferObject));
    }
#ifndef NOVST
    if (this->plugin != NULL) {
        delete this->plugin;
        this->plugin = NULL;
    }
#endif
    resetScene();
}

Options *Synthmania::getOptions() { return options.get(); }

void Synthmania::keyCallback(GLFWwindow *win, int key, int scancode, int action,
                             int mods) {
    Synthmania *game = (Synthmania *)glfwGetWindowUserPointer(win);
    Window *window = game->getWindow();
    if (game == NULL || action != GLFW_PRESS) return;

    if (key == GLFW_KEY_TAB) {
        game->autoPlay = !game->autoPlay;
        return;
    }
    if (key == GLFW_KEY_RIGHT) {
        game->setTimeMicros(game->getCurrentTimeMicros() + 2000000);
        return;
    }
    if (key == GLFW_KEY_ESCAPE) {
        game->endSong();
        game->loadMenu("song select");
        return;
    }
    if (game->autoPlay) return;
    /*int k = 0;
    int keys[] = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_X, GLFW_KEY_D,
                  GLFW_KEY_C, GLFW_KEY_V, GLFW_KEY_G, GLFW_KEY_B,
                  GLFW_KEY_H, GLFW_KEY_N, GLFW_KEY_J, GLFW_KEY_M};
    for (k = 0; k < 12; k++) {
        if (keys[k] == key) break;
    }*/
    for (Note *note : game->notes) {
        // This is ADOFAI for now
        if (  // note->getPitch() == k &&
            note->getStatus() == WAITING &&
            std::abs(note->getTime() - game->getCurrentTimeMicros()) <
                (game->drum
                     ? DRUM_HIT_WINDOW
                     : HIT_WINDOW)  // && next note not skipped/close?? or just
                                    // set OD to a value that prevents the need
                                    // for notelock idk hmm
        ) {
            game->noteHit(note);
            break;
        }
    }
}

void Synthmania::noteHit(Note *note) {
    if (note->getStatus() != WAITING) return;
    int64_t time = getCurrentTimeMicros();
    int64_t delta = autoPlay ? 0 : time - note->getTime();
    Precision *prec = new Precision(
        getTextureByName(renderer->getTextures(), "precision_tick"), "tick",
        time, delta);
    prec->setSize({0.1f, 0.4f});
    prec->setPosition({0, 0.9f});
    addGui(prec);
    delta = std::clamp<int64_t>(delta, 0, note->getTotalDuration() / 2);
    // TODO velocity
    if (!drum) {
#ifndef NOVST
        if (plugin != NULL)
            plugin->playNote(note->getPitch(), 90,
                             time + note->getTotalDuration() - delta);
        else
#endif
            playPianoSound(note->getPitch());
    } /*else
        playDrumSound(note->getPitch());*/
    note->setStatus(HIT);
    note->kill(time + note->getTotalDuration());

    std::string text = "Good!";
    int i = 0;
    std::string name = "hit_";
    name.append(std::to_string((size_t)note));
    name.append("_");
    /*for (Text t : renderer->getTextHandler()->createText(
             text, "Stupid", 11,
             {-2, (double)(-.25 + line->getPosition().y +
                           line->getSize().y / 2.)})) {
        Gui *gui =
            new Gui(t.character.texture, (name + std::to_string(i++)).c_str());
        gui->addEffect(new GraphicalEffect(applyTemp));
        gui->setColor({.3, 1, .3, 1});
        gui->setNegate(true);
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        addGui(gui);
    }*/
}

void Synthmania::playDrumSound(unsigned char pitch) {
    switch (pitch) {
        case MidiPercussion::KICK:
            audio->playSound("kick");
            break;
        case MidiPercussion::SNARE:
            audio->playSound("snare");
            break;
        case MidiPercussion::RIDE:
            audio->playSound("ride");
            break;
        case MidiPercussion::CRASH:
            audio->playSound("crash");
            break;
        case MidiPercussion::HAT:
            audio->playSound("hat");
            break;
        case MidiPercussion::TOM_HIGH:
            audio->playSound("tom-high");
            break;
        case MidiPercussion::TOM_MID:
            audio->playSound("tom-mid");
            break;
        case MidiPercussion::TOM_LOW:
            audio->playSound("tom-low");
            break;
        default:
            std::cout << "bruh : " << (unsigned int)pitch << "\n";
    }
}

void Synthmania::playPianoSound(unsigned char pitch) {
    AudioSource *source = audio->playSound("piano");
    float p = std::pow(1. + 1. / 12., (pitch - 60));
    source->setPitch(p);
}

void Synthmania::addGui(Gui *gui) {
    Game::addGui(gui);
    if (mod != NULL) mod->onSpawn(gui);
}

void Synthmania::noteMiss(Note *note) {
    if (note->getStatus() != WAITING) return;
    int64_t time = getCurrentTimeMicros();
    note->setStatus(MISSED);
    note->kill(time);

    std::string text = "BAD";
    int i = 0;
    for (Text t : renderer->getTextHandler()->createText(
             text, "Stupid", 11,
             {-2, (double)(-.25 + line->getPosition().y +
                           line->getSize().y / 2.)})) {
        std::string name = "miss_";
        name.append(std::to_string((size_t)note));
        name.append("_");
        name.append(std::to_string(i++));
        Gui *gui = new Gui(t.character.texture, name.c_str());
        gui->addEffect(new GraphicalEffect(applyTemp));
        gui->setColor({1, 0, 0, 1});
        gui->setNegate(true);
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        addGui(gui);
    }
}

void Synthmania::setTimeMicros(int64_t time) {
    Game::setTimeMicros(time);
    if (music != NULL)
        music->setSampleOffset((getCurrentTimeMicros() + startTime) * 44100 /
                               1000000.f);
}

void Synthmania::resetScene() {
    Game::resetScene();
    notes.clear();
    if (mod != NULL) {
        delete mod;
        mod = NULL;
    }
}

void Synthmania::update() {
    if ((music != NULL && music->getState() != AL_PLAYING) ||
        (music == NULL && menu == NULL && this->notes.size() == 0)) {
        endSong();
        loadMenu("main");
        return;
    }
    int64_t time_from_start = getCurrentTimeMicros();
    // std::cout << std::dec << time_from_start << " ";
    if (autoPlay)
        for (Note *note : notes)
            if (note->getTime() <= time_from_start) noteHit(note);

#ifndef NOVST
    if (plugin != NULL) plugin->update(time_from_start);
#endif
    std::vector<Gui *> toDestroy;
    for (Gui *g : guis) {
        if (g->update(time_from_start) || g->isDestroyed()) {
            g->setDestroyed();
            toDestroy.push_back(g);
        }
        g->updateGraphics(time_from_start);
    }
    for (Gui *g : toDestroy) {
        for (auto iter = notes.begin(); iter != notes.end(); iter++)
            if (g == *iter) {
                notes.erase(iter);
                break;
            }
        for (std::vector<Gui *>::iterator iter = guis.begin();
             iter != guis.end(); iter++)
            if (g == *iter) {
                guis.erase(iter);
                delete g;
                break;
            }
    }
    if (mod != NULL) mod->update(time_from_start);
    // TODO refactor this shit the game should not be handling note logic
    // Or should it ? *vsauce music plays*
    // Accshually, if I only check a few notes (the first 5 in array for
    // instance) and if there cannot be more than this amount in the hit window
    // then I can check shit semi-optimally :nerd:
    for (Note *n : notes) {
        if (n->justMissed()) {
            noteMiss(n);
            continue;
        }

        if (n->getStatus() == HIT &&
            time_from_start >= n->getTime() + n->getTotalDuration()) {
            n->setStatus(FINISHED);
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
    if (!autoPlay) {
        Message m = handler->getMessage();
        while (m.type != libremidi::message_type::INVALID) {
            if (m.type == libremidi::message_type::NOTE_ON) {
                short got_one = false;
                if (drum) {
                    playDrumSound(m.data1);
                }
                for (Note *note : notes) {
                    if (note->getStatus() == WAITING &&
                        note->getPitch() == m.data1 &&
                        std::abs(note->getTime() - time_from_start) <
                            (drum ? DRUM_HIT_WINDOW : HIT_WINDOW)) {
                        noteHit(note);
                        got_one = true;
                        break;
                    }
                }
                if (!drum && !got_one) {
                    for (Note *note : notes) {
                        if (note->getStatus() == WAITING &&
                            std::abs(note->getTime() - time_from_start) <
                                (drum ? DRUM_HIT_WINDOW : HIT_WINDOW)) {
                            noteMiss(note);
                        }
                    }
                }
            }
            m = handler->getMessage();
        }
    }
    if (music != NULL) {  // audio latency study
        long long j = music->getSampleOffset() * 1000000L;
        int64_t a = getCurrentTimeMicros();
        int64_t res = j / (uint64_t)44100 - this->startTime;
        int err;
        if ((err = alGetError()) != AL_NO_ERROR)
            std::cerr << "OpenAL error when getting sample offset:" << err
                      << std::endl;
        int64_t b = res - a;
        if (b < 0) b = -b;
        // std::cout << std::dec << audioLeniency << ':' << j << ',' << b << ','
        //           << a << "\n";
        // If music gets more than 10ms off reset it
        // Most people can play with 10ms off right? (I'm sorry rythm gamers)
        if (b > audioLeniency) setTimeMicros(a);
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
    if (music != NULL) music->setGain(musicVol);
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
    if (mod != NULL) return mod->updateUBO(ubo, getCurrentTimeMicros());
    return sizeof(UniformBufferObject);
}

void Synthmania::freeUBO(void *&ubo) {
    if (mod != NULL) mod->freeUBO(ubo);
}

size_t Synthmania::updateFinalUBO(void *&ubo) {
    if (mod != NULL) return mod->updateFinalUBO(ubo, getCurrentTimeMicros());
    return sizeof(UniformBufferObject);
}

void Synthmania::freeFinalUBO(void *&ubo) {
    if (mod != NULL) mod->freeFinalUBO(ubo);
}

Chart Synthmania::getChart() { return chart; }

TrackPartition Synthmania::getPartition() { return partition; }

#ifndef NOVST
AudioPluginHandler *Synthmania::getPluginHandler() { return plugin; }
#endif

std::string Synthmania::getSongFolder() { return songFolder; }

Synthmania::~Synthmania() {
    if (mod != NULL) delete mod;
#ifndef NOVST
    if (plugin != NULL) delete plugin;
#endif

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
                                           new float[]{shake * (float)size}));
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