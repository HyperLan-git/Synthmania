#include "PlayMode.hpp"

PlayMode::PlayMode(Synthmania *game, std::string songFolder)
    : game(game), songFolder(songFolder) {
    game->getWindow()->setKeycallback(PlayMode::keyCallback);
    std::string json = songFolder;
    json.append("/sdata.json");
    chart = readChart(json.c_str());
    diff = chart.diffs[0];
    game->resetClock();  //<==> game->startTime = chart.offset;
    Renderer *renderer = game->getRenderer();
    // TODO could be in a function
    if (chart.animation.compare("None") != 0) {
        std::cout << "loading animation" << std::endl;
        void *shared = loadShared(songFolder + "/" + chart.animation);
        char *e = NULL;  // TODO dlerror();
        if (e != NULL) {
            std::cerr << "Error while loading anim ! " << e << "\n";
        } else {
            ChartHandler *(*f)(Gamemode *) = (ChartHandler * (*)(Gamemode *))
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
    partition = game->getMidiHandler()->readMidi(path.c_str());
    this->drum = partition.drumming;
    Model *model =
        new Model("resources/models/room.obj", renderer->getPhysicalDevice(),
                  renderer->getDevice());
    std::vector<ImageView *> textures = renderer->getTextures();
    std::shared_ptr<Entity> la_creatura76 = std::make_shared<Entity>(
        model, getTextureByName(textures, "room"), "Bob");
    renderer->addModel(model);
    game->addEntity(la_creatura76);
    Key k = drum ? Key::DRUM : Key::SOL;
    std::string keyName = (drum) ? "drum_key" : "sol_key";
    std::shared_ptr<Gui> part = std::make_shared<Gui>(
                             getTextureByName(textures, "partition"),
                             "partition"),
                         bg = std::make_shared<Gui>(
                             getTextureByName(textures, "background"), "bg"),
                         precision = std::make_shared<Gui>(
                             getTextureByName(textures, "precision"),
                             "precision"),
                         key = std::make_shared<Gui>(
                             getTextureByName(textures, keyName), "key");
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
    game->addGui(bg);
    game->addGui(part);
    game->addGui(key);
    bg->setSize({5, 30});
    part->setSize({5, 1});
    precision->setSize({1.5f, 0.5f});
    precision->setPosition({0, 0.9f});
    std::shared_ptr<Judgement> bar =
        std::make_shared<Judgement>("judgement", textures, partition);
    line = bar;
    bar->setPosition({-1.3f, bar->getPosition().y});
    bar->setSize({0.25f, 1.f});
    game->addTGui(bar);
    std::vector<std::shared_ptr<Gui>> tempNotes;
    // TODO make this less cringe
    {
        std::vector<std::shared_ptr<Note>> keepAlive;
        for (MidiNote note : partition.notes) {
            std::string name = "Note_";
            std::string hash = std::to_string(std::hash<MidiNote>()(note));
            name += hash;
            double totalDuration =
                note.length / (long double)partition.MPQ / 4.;
            std::vector<double> cutDown = splitDuration(totalDuration);
            short firstDots = 0;
            for (int i = 1; i < cutDown.size(); i++)
                if (cutDown.size() > 1 && cutDown[i] == cutDown[i - 1] / 2.)
                    firstDots = i;
            for (int i = 0; i < firstDots; i++) {
                cutDown[1] += cutDown[0];
                cutDown.erase(cutDown.begin());
            }
            std::shared_ptr<Note> n = std::make_shared<Note>(
                name.c_str(), note.timestamp, note.note, totalDuration,
                cutDown[0], partition.MPQ, textures, k);
            keepAlive.push_back(n);
            notes.push_back(n);
            int diff = getDifferenceFromC4(transposePitch(k, note.note)) +
                       getOffset(k);
            if (diff <= 0 || diff >= 12) {
                bool up = diff >= 12;
                for (int i = up ? 12 : 0;
                     (up && i <= diff) || (!up && i >= diff);
                     up ? i += 2 : i -= 2) {
                    std::string barName = "Bar_";
                    barName.append(hash);
                    std::shared_ptr<PartitionNotation> readability =
                        std::make_shared<PartitionNotation>(
                            barName.c_str(), note.timestamp, (0.5 - 0.083 * i),
                            getTextureByName(textures, "bar"));
                    readability->setPosition({0, readability->getPosition().y});
                    readability->setSize({0.25, 0.15});
                    game->addTGui(readability);
                }
            }

            double p = .2;
            for (int i = 0; i < firstDots; i++) {
                std::shared_ptr<ParentedGui> dot =
                    std::make_shared<ParentedGui>(
                        getTextureByName(textures, "dot"), name.c_str(), n);
                dot->setSize({.05, .05});
                dot->setPosition({p, 0});
                tempNotes.push_back(dot);
                p += .2;
            }

            if (k != Key::DRUM && !isFromCMajor(note.note)) {
                std::string sharpName = "Sharp_";
                sharpName.append(hash);
                std::shared_ptr<ParentedGui> sharp =
                    std::make_shared<ParentedGui>(
                        getTextureByName(textures, "sharp"), sharpName.c_str(),
                        n);
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
                std::shared_ptr<ParentedGui> p = std::make_shared<ParentedGui>(
                    getTextureForNote(textures, note.note, d, k), name2.c_str(),
                    n);
                glm::vec2 temp = getSizeAndLocForNote(d, k, note.note);
                p->setPosition({(t - note.timestamp) / 300000.f, 0});
                p->addEffect(
                    new GraphicalEffect(applyOffset, new float[2]{0, temp.x}));
                p->setSize({temp.y, temp.y});
                tempNotes.push_back(p);
                std::shared_ptr<ParentedGui> arc =
                    std::make_shared<ParentedGui>(
                        getTextureByName(textures, "arc"), name2.c_str(), n);
                arc->setPosition(
                    {(((t + last) / 2) - note.timestamp) / 300000.f, 0});
                arc->addEffect(new GraphicalEffect(
                    applyOffset, new float[2]{0, temp.x - .15f}));
                arc->setSize({(t - last) / 350000.f, .15f});
                tempNotes.push_back(arc);
                last = t;
                t += d * partition.MPQ * 4;
            }
        }
        std::vector<std::shared_ptr<Gui>> tmp;
        for (std::weak_ptr<Note> &note : notes)
            tmp.push_back(std::shared_ptr<Note>(note));

        if (tmp.size() > 0) sortGuis(tmp, cmpGuisByTexture);
        if (tempNotes.size() > 0) sortGuis(tempNotes, cmpGuisByTexture);
        for (std::shared_ptr<Gui> &note : tmp) game->addGui(note);
    }
    for (std::shared_ptr<Gui> &gui : tempNotes) game->addGui(gui);
    music = NULL;
    AudioHandler *audio = game->getAudioHandler();
    if (chart.audio.compare("None") != 0) {
        std::string wav = songFolder;
        wav.append("/");
        wav.append(chart.audio);
        AudioBuffer *buffer = new AudioBuffer(wav);

        audio->addSound("song", buffer);
        music = audio->playSound("song");
        music->setGain(game->getMusicVolume());
        music->setDestroyOnFinished(false);
    }
#ifdef VST
    std::string pdata = "None";
    if (chart.plugindata.compare("None") != 0) {
        pdata = songFolder;
        pdata.append("/");
        pdata.append(chart.plugindata);
    }
    path = game->getPlugin(chart.plugin);
    if (!path.empty())
        plugin = new AudioPluginHandler(path, audio, pdata);
    else if (chart.plugin.compare("None") != 0)
        std::cout << "Could not find plugin : " << chart.plugin << std::endl;
#endif
    std::string text = chart.name;
    text.append(" by ");
    text.append(chart.artist);

    for (std::shared_ptr<Gui> &g :
         printShadowedString(text, renderer->getTextHandler(), "title_",
                             "Stupid", 11, {-1.75, -.9}, {.2, .2, 1, 1}))
        game->addGui(g);

    /*for (Gui *g : printShakingString("ANGERY", renderer, "scary_", "Stupid",
       22, {0, 0}, .001, {1, 0, 0, 1})) game->addGui(g);*/
    // Needs to be above everything else
    game->addGui(precision);
    game->setTimeMicros(-chart.offset);
}

bool PlayMode::update() {
    if ((music != NULL && music->getState() != AL_PLAYING) ||
        (music == NULL && this->notes.size() == 0)) {
        return true;
    }
    int64_t time_from_start = game->getCurrentTimeMicros();
    // std::cout << std::dec << time_from_start << " ";
    if (autoPlay)
        for (std::weak_ptr<Note> &note : notes) {
            if (note.expired()) continue;
            std::shared_ptr<Note> n(note);
            if (n->getStatus() == WAITING && n->getTime() <= time_from_start) {
                noteHit(n);
                if (drum) playDrumSound(n->getPitch());
            }
        }

#ifdef VST
    if (plugin) plugin->update(time_from_start);
#endif
    if (mod) mod->update(time_from_start);
    // TODO refactor this shit the game should not be handling note logic
    // Or should it ? *vsauce music plays*
    // Accshually, if I only check a few notes (the first 5 in array for
    // instance) and if there cannot be more than this amount in the hit window
    // then I can check shit semi-optimally :nerd:
    for (std::weak_ptr<Note> &note : notes) {
        if (note.expired()) continue;
        std::shared_ptr<Note> n(note);
        if (n->justMissed()) {
            noteMiss(n);
            continue;
        }

        if (n->getStatus() == HIT &&
            time_from_start >= n->getTime() + n->getTotalDuration()) {
            n->setStatus(FINISHED);
        }
    }

    if (!autoPlay) {
        Message m = game->getMidiHandler()->getMessage();
        while (m.type != libremidi::message_type::INVALID) {
            if (m.type == libremidi::message_type::NOTE_ON) {
                short got_one = false;
                if (drum) {
                    playDrumSound(m.data1);
                }
                for (const std::weak_ptr<Note> &note : notes) {
                    if (note.expired()) continue;
                    std::shared_ptr<Note> n(note);
                    if (n->getStatus() == WAITING && n->getPitch() == m.data1 &&
                        std::abs(n->getTime() - time_from_start) <
                            (drum ? DRUM_HIT_WINDOW : HIT_WINDOW)) {
                        noteHit(n);
                        got_one = true;
                        break;
                    }
                }
                if (!drum && !got_one) {
                    for (const std::weak_ptr<Note> &note : notes) {
                        if (note.expired()) continue;
                        std::shared_ptr<Note> n(note);
                        if (n->getStatus() == WAITING &&
                            std::abs(n->getTime() - time_from_start) <
                                (drum ? DRUM_HIT_WINDOW : HIT_WINDOW)) {
                            noteMiss(n);
                        }
                    }
                }
            }
            m = game->getMidiHandler()->getMessage();
        }
    }
    this->notes.erase(
        std::remove_if(notes.begin(), notes.end(),
                       [](std::weak_ptr<Note> &n) { return n.expired(); }),
        this->notes.end());
    if (music != NULL) {  // audio latency study
        long long j = music->getSampleOffset() * 1000000L;
        int64_t a = game->getCurrentTimeMicros();
        int64_t res = j / (uint64_t)44100 - chart.offset;
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
        if (b > game->getAudioLeniency()) game->setTimeMicros(a);
    }
    return false;
}

void PlayMode::noteHit(const std::shared_ptr<Note> &note) {
    if (note->getStatus() != WAITING) return;
    int64_t time = game->getCurrentTimeMicros();
    int64_t delta = autoPlay ? 0 : time - note->getTime();
    std::shared_ptr<Gui> prec = std::make_shared<Precision>(
        getTextureByName(game->getRenderer()->getTextures(), "precision_tick"),
        "tick", time, delta);
    prec->setSize({0.1f, 0.4f});
    prec->setPosition({0, 0.9f});
    prec->updateGraphics(time);
    game->addGui(prec);
    delta = std::clamp<int64_t>(delta, 0, note->getTotalDuration() / 2);
    // TODO velocity
    if (!drum) {
#ifdef VST
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
    name.append(std::to_string((size_t)note.get()));
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

void PlayMode::noteMiss(const std::shared_ptr<Note> &note) {
    if (note->getStatus() != WAITING) return;
    int64_t time = game->getCurrentTimeMicros();
    note->setStatus(MISSED);
    note->kill(time);

    std::string text = "BAD";
    int i = 0;
    for (Text t : game->getRenderer()->getTextHandler()->createText(
             text, "Stupid", 11,
             {-2, (double)(-.25 + line->getPosition().y +
                           line->getSize().y / 2.)})) {
        std::string name = "miss_";
        name.append(std::to_string((size_t)note.get()));
        name.append("_");
        name.append(std::to_string(i++));
        std::shared_ptr<Gui> gui =
            std::make_shared<Gui>(t.character.texture, name.c_str());
        gui->addEffect(new GraphicalEffect(applyTemp));
        gui->setColor({1, 0, 0, 1});
        gui->setNegate(true);
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        game->addGui(gui);
    }
}

Chart PlayMode::getChart() { return chart; }
TrackPartition PlayMode::getPartition() { return partition; }

std::string PlayMode::getSongFolder() { return songFolder; }

void PlayMode::playDrumSound(unsigned char pitch) {
    AudioHandler *audio = game->getAudioHandler();
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

void PlayMode::playPianoSound(unsigned char pitch) {
    AudioSource *source = game->getAudioHandler()->playSound("piano");
    float p = std::pow(1. + 1. / 12., (pitch - 60));
    source->setPitch(p);
}

void PlayMode::spawnNote(MidiNote note) {}

PlayMode::~PlayMode() {
    game->getAudioHandler()->removeSound(this->music);
    if (this->mod) {
        delete this->mod;
        Renderer *renderer = game->getRenderer();
        renderer->loadGuiShaders("bin/gui.vert.spv", "bin/def.geom.spv",
                                 "bin/def.frag.spv",
                                 sizeof(UniformBufferObject));
        renderer->loadFinalShaders("bin/pass.vert.spv", "bin/pass.geom.spv",
                                   "bin/pass.frag.spv",
                                   sizeof(UniformBufferObject));
    }
#ifdef VST
    if (this->plugin) delete this->plugin;
#endif
    game->resetScene();
    game->loadMenu("main");
}

void PlayMode::keyCallback(GLFWwindow *win, int key, int scancode, int action,
                           int mods) {
    Synthmania *game = (Synthmania *)glfwGetWindowUserPointer(win);
    Window *window = game->getWindow();
    if (game == NULL || action != GLFW_PRESS) return;
    PlayMode *inst = dynamic_cast<PlayMode *>(game->getGamemode());

    if (key == GLFW_KEY_TAB) {
        inst->autoPlay = !inst->autoPlay;
        return;
    }
    if (key == GLFW_KEY_RIGHT) {
        game->setTimeMicros(game->getCurrentTimeMicros() + 2000000);
        return;
    }
    if (key == GLFW_KEY_ESCAPE) {
        game->setGamemode(NULL);
        game->loadMenu("song select");
        return;
    }
    if (inst->autoPlay) return;
    /*int k = 0;
    int keys[] = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_X, GLFW_KEY_D,
                  GLFW_KEY_C, GLFW_KEY_V, GLFW_KEY_G, GLFW_KEY_B,
                  GLFW_KEY_H, GLFW_KEY_N, GLFW_KEY_J, GLFW_KEY_M};
    for (k = 0; k < 12; k++) {
        if (keys[k] == key) break;
    }*/
    for (std::weak_ptr<Note> &note : inst->notes) {
        if (note.expired()) continue;
        std::shared_ptr<Note> n(note);
        // This is ADOFAI for now
        if (  // b->getPitch() == k &&
            n->getStatus() == WAITING &&
            std::abs(n->getTime() - game->getCurrentTimeMicros()) <
                (inst->drum
                     ? DRUM_HIT_WINDOW
                     : HIT_WINDOW)  // && next note not skipped/close?? or
                                    // just set OD to a value that prevents
                                    // the need for notelock idk hmm
        ) {
            inst->noteHit(n);
            if (inst->drum) inst->playDrumSound(n->getPitch());
            break;
        }
    }
}

void PlayMode::onClockAdjust(int64_t t) {
    if (music)
        music->setSampleOffset((game->getCurrentTimeMicros() + chart.offset) *
                               44100 / 1000000.f);
}

void PlayMode::onConfigChange() {
    if (music) music->setGain(game->getMusicVolume());
}

size_t PlayMode::updateUBO(void *&ubo) {
    if (mod) return mod->updateUBO(ubo, game->getCurrentTimeMicros());
    return sizeof(UniformBufferObject);
}

void PlayMode::freeUBO(void *&ubo) {
    if (mod) mod->freeUBO(ubo);
}

size_t PlayMode::updateFinalUBO(void *&ubo) {
    if (mod) return mod->updateFinalUBO(ubo, game->getCurrentTimeMicros());
    return sizeof(UniformBufferObject);
}

void PlayMode::freeFinalUBO(void *&ubo) {
    if (mod) mod->freeFinalUBO(ubo);
}

void PlayMode::onSpawn(std::shared_ptr<Gui> &g) {
    if (mod) mod->onSpawn(g);
}