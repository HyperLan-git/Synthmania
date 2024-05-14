#include "PlayMode.hpp"

PlayMode::PlayMode(Synthmania &game, std::string songFolder)
    : game(game), songFolder(songFolder), score() {
    game.getWindow().setKeycallback(PlayMode::keyCallback);
    std::string json = songFolder;
    json.append("/sdata.json");
    chart = readChart(json);
    diff = chart.diffs[0];
    game.resetClock();  //<==> game->startTime = chart.offset;
    Renderer &renderer = game.getRenderer();
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
                this->mod = std::unique_ptr<ChartHandler>(f(this));  // Shit
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
            renderer.loadGuiShaders(v, g, f, UBOSize);
        if (!v_f.empty() || !g_f.empty() || !f_f.empty())
            renderer.loadFinalShaders(v_f, g_f, f_f, fUBOSize);
    }
    std::string path = songFolder;
    path.append("/");
    path.append(diff.midi);
    partition = game.getMidiHandler().readMidi(path);
    this->drum = partition.drumming;
    Model model =
        loadFromFile(renderer.getDevice(), "resources/models/room.obj");
    std::shared_ptr<Entity> la_creatura76 = std::make_shared<Entity>(
        renderer.addModel(std::move(model)), Texture("room"), "Bob");
    game.addEntity(la_creatura76);
    Key k = drum ? Key::DRUM : Key::SOL;
    std::string keyName = (drum) ? "drum_key" : "sol_key";
    std::shared_ptr<Gui> part = std::make_shared<Gui>(Texture("partition"),
                                                      "partition"),
                         bg =
                             std::make_shared<Gui>(Texture("background"), "bg"),
                         precision = std::make_shared<Gui>(Texture("precision"),
                                                           "precision"),
                         key = std::make_shared<Gui>(Texture(keyName), "key");
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
    game.addGui(bg);
    game.addGui(part);
    game.addGui(key);
    bg->setSize({5, 30});
    part->setSize({5, 1});
    precision->setSize({1.5f, 0.5f});
    precision->setPosition({0, 0.9f});
    std::shared_ptr<Judgement> bar = std::make_shared<Judgement>(
        "judgement", Texture("judgement_line"), partition);
    line = bar;
    bar->setPosition({-1.3f, bar->getPosition().y});
    bar->setSize({0.25f, 1.f});
    game.addTGui(bar);
    // FIXME make this less cringe
    {
        std::vector<std::shared_ptr<Gui>> tempNotes;
        std::vector<std::shared_ptr<Note>> keepAlive;
        std::shared_ptr<PartitionNotation> bar;
        int groupedNotes = 0, beams = 0;
        std::shared_ptr<Note> groupStart;
        for (auto iter = partition.notes.cbegin();
             iter != partition.notes.cend(); iter++) {
            const MidiNote &note = *iter;
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
                name, note.timestamp, note.note, totalDuration, cutDown[0],
                partition.MPQ, k, partition.signature);
            keepAlive.push_back(n);
            notes.push_back(n);

            // Beaming
            /* XXX Shitcode that needs to be removed why am I stupid?

            if (groupedNotes == 0 && totalDuration <= .125) {
                int groupLimit = 1;
                beams = 0;
                groupStart = n;
                if (totalDuration == .125) {
                    groupLimit = 4;
                    beams = 1;
                }
                double dur = totalDuration;
                while (dur < .125) {
                    groupLimit *= 2;
                    dur *= 2;
                    beams++;
                }
                auto it2 = iter;
                uint64_t time = note.timestamp + note.length;
                while ((++it2) != partition.notes.cend() &&
                       groupLimit > groupedNotes++) {
                    if (it2->timestamp != time || it2->length != note.length)
                        break;
                    time += note.length;
                }
            }
            if (groupedNotes >= 0) {
                if (--groupedNotes == 0) {
                    for (int b = 0; b < beams; b++) {
                        glm::vec2 sz =
                            getSizeAndLocForNote(note.length, k, note.note);
                        bar = std::make_shared<PartitionNotation>(
                            "linkbar", note.timestamp, 0, Texture("bar"));
                        float x = getNotationPosition(n->getTime()) -
                                  getNotationPosition(groupStart->getTime()),
                              y = n->getPosition().y -
                                  groupStart->getPosition().y;
                        bar->setSize({x, .3});
                        bar->setRotation(std::atan2(y, x));
                        bar->addEffect(GraphicalEffect(
                            applyOffset, {-n->getSize().x / 2,
                                          n->getPosition().y - n->getSize().y -
                                              y / 2 + b * .05f}));
                        // bar->addEffect(GraphicalEffect(applyShaking));
                        game.addTGui(bar);
                    }
                }
            }*/

            // Reading bars
            int diff = getDifferenceFromC4(transposePitch(k, note.note),
                                           partition.signature) +
                       getOffset(k);
            if (diff <= 0 || diff >= 12) {
                bool up = diff >= 12;
                for (int i = up ? 12 : 0;
                     (up && i <= diff) || (!up && i >= diff);
                     up ? i += 2 : i -= 2) {
                    std::string barName = "Bar_" + hash;
                    std::shared_ptr<PartitionNotation> readability =
                        std::make_shared<PartitionNotation>(
                            barName, note.timestamp, (0.5 - 0.083 * i),
                            Texture("bar"));
                    readability->setPosition({0, readability->getPosition().y});
                    readability->setSize({0.25, 0.15});
                    game.addTGui(readability);
                }
            }

            // Dotted time
            double p = .2;
            for (int i = 0; i < firstDots; i++) {
                std::shared_ptr<ParentedGui> dot =
                    std::make_shared<ParentedGui>(Texture("dot"), name, n);
                dot->setSize({.05, .05});
                dot->setPosition({p, 0});
                tempNotes.push_back(dot);
                p += .2;
            }

            // Sharp symbol thing
            Accidental a = getAccidental(note.note, partition.signature);
            if (k != Key::DRUM && a == Accidental::SHARP) {
                std::string sharpName = "Sharp_" + hash;
                std::shared_ptr<ParentedGui> sharp =
                    std::make_shared<ParentedGui>(Texture("sharp"), sharpName,
                                                  n);
                sharp->setPosition({-0.25, 0});
                sharp->setSize({0.25, 0.25});
                tempNotes.push_back(sharp);
            }
            uint64_t last = note.timestamp;
            uint64_t t = note.timestamp + cutDown[0] * partition.MPQ * 4;
            for (int i = 1; i < cutDown.size(); i++) {
                std::string name2 = name + "_" + std::to_string(i);
                double d = cutDown[i];
                std::shared_ptr<ParentedGui> p = std::make_shared<ParentedGui>(
                    getTextureForNote(note.note, d, k, false), name2, n);
                glm::vec2 temp = getSizeAndLocForNote(d, k, note.note);
                p->setPosition({(t - note.timestamp) / 300000.f, 0});
                p->addEffect(GraphicalEffect(
                    applyOffset, std::initializer_list<float>{0, temp.x}));
                p->setSize({temp.y, temp.y});
                tempNotes.push_back(p);
                std::shared_ptr<ParentedGui> arc =
                    std::make_shared<ParentedGui>(Texture("arc"), name2, n);
                arc->setPosition(
                    {(((t + last) / 2) - note.timestamp) / 300000.f, 0});
                arc->addEffect(GraphicalEffect(
                    applyOffset,
                    std::initializer_list<float>{0, temp.x - .15f}));
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
        for (std::shared_ptr<Gui> &note : tmp) game.addGui(note);
        for (std::shared_ptr<Gui> &gui : tempNotes) game.addGui(gui);
    }
    std::shared_ptr<Note> lastNote(*notes.rbegin());
    for (int i = 0; i < lastNote->getTime() / partition.MPQ / 4; i++) {
        auto bar = std::make_shared<PartitionNotation>(
            "measurebar_" + std::to_string(i), i * partition.MPQ * 4, 0,
            Texture("bar"));
        bar->setRotation(glm::half_pi<float>());
        bar->setSize({.67, .2});
        bar->addEffect(GraphicalEffect(applyOffset, {-.15, 0}));
        game.addTGui(bar);
    }
    AudioHandler &audio = game.getAudioHandler();
    if (chart.audio.compare("None") != 0) {
        std::string wav = songFolder;
        wav.append("/");
        wav.append(chart.audio);

        audio.addSound("song", AudioBuffer(wav));
        music = std::make_optional<std::reference_wrapper<AudioSource>>(
            audio.playSound("song"));
        music->get().setGain(game.getMusicVolume());
        music->get().setDestroyOnFinished(false);
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
         renderer.getTextHandler().printShadowedString(
             text, "title_", "Stupid", 11, {-1.75, -.9}, {.2, .2, 1, 1}))
        game.addGui(g);

    this->autoplayText = renderer.getTextHandler().printString(
        "AUTOPLAY", "autoplay", "Stupid", 16, glm::vec2(.8, 1), glm::vec4(0));

    for (std::shared_ptr<Gui> &g : this->autoplayText) game.addGui(g);

    // Needs to be above everything else
    game.addGui(precision);
    game.setTimeMicros(-chart.offset);
    start = this->game.getMidiHandler().getTime() + chart.offset;
    score.misses = notes.size();
    score_per_note =
        1000000 / notes.size() + (1000000 % notes.size() == 0 ? 0 : 1);
}

bool PlayMode::update() {
    if ((music && music->get().getState() != AL_PLAYING) &&
        this->notes.size() == 0) {
        std::cout << "state:" << music->get().getState() << std::endl;
        return true;
    }
    int64_t time_from_start = game.getCurrentTimeMicros();
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
    int64_t t_stop = time_from_start + 10000000;
    for (std::weak_ptr<Note> &note : notes) {
        if (note.expired()) continue;
        std::shared_ptr<Note> n(note);
        if (n->justMissed()) {
            noteMiss(n);
            continue;
        }

        if (n->getTime() > t_stop) break;
        if (n->getStatus() == HIT &&
            time_from_start >= n->getTime() + n->getTotalDuration()) {
            n->setStatus(FINISHED);
        }
    }

    if (!autoPlay) {
        Message m = game.getMidiHandler().getMessage();
        while (m.type != libremidi::message_type::INVALID) {
            if (m.type == libremidi::message_type::NOTE_ON &&
                m.data2 > 0) {  // vel
                short got_one = false;
                if (drum) playDrumSound(m.data1);

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
            m = game.getMidiHandler().getMessage();
        }
    }
    this->notes.erase(
        std::remove_if(notes.begin(), notes.end(),
                       [](std::weak_ptr<Note> &n) { return n.expired(); }),
        this->notes.end());
    if (music) {  // audio latency study
        int err;
        long long j = music->get().getSampleOffset() * 1000000L;
        int64_t a = game.getCurrentTimeMicros();
        int64_t res = j / (uint64_t)44100 - chart.offset;
        OPENAL_DEBUG("getting sample offset");
        int64_t b = res - a;
        if (b < 0) b = -b;
        // std::cout << std::dec << audioLeniency << ':' << j << ',' << b << ','
        //           << a << "\n";
        // If music gets more than 10ms off reset it
        // Most people can play with 10ms off right? (I'm sorry rythm gamers)
        if (b > game.getAudioLeniency()) game.setTimeMicros(a);
    }
    return false;
}

void PlayMode::noteHit(const std::shared_ptr<Note> &note) {
    if (note->getStatus() != WAITING) return;
    int64_t time = game.getCurrentTimeMicros();
    int64_t delta = autoPlay ? 0 : time - note->getTime(), d2 = std::abs(delta);
    std::shared_ptr<Gui> prec = std::make_shared<Precision>(
        Texture("precision_tick"), "tick", time, delta);
    float x = prec->getPosition().x;
    prec->setSize({0.1f, 0.4f});
    prec->setPosition({x, 0.9f});
    game.addGui(prec);
    bool perfect = d2 < 32000, great = d2 < 64000, good = d2 < 100000;
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

    std::string text;
    glm::vec4 color;
    score.misses--;
    if (perfect) {
        text = "PERFECT";
        score.score += score_per_note;
        color = glm::vec4(.2, .2, 1, 1);
        score.perfects++;
    } else if (great) {
        text = "Great!";
        score.score += score_per_note * .7;
        color = glm::vec4(.3, 1, .3, 1);
        score.near++;
    } else if (good) {
        score.score += score_per_note * .5;
        text = "Good";
        color = glm::vec4(.3, 1, .3, 1);
        score.far++;
    } else {
        text = "BAD";
        color = glm::vec4(.3, .3, .3, 1);
        score.misses++;
    }
    int i = 0;
    std::string name = "hit_" + std::to_string((size_t)note.get()) + "_";
    for (Text t : this->game.getRenderer().getTextHandler().createText(
             text, "Stupid", 11,
             {-1.8, line->getPosition().y - line->getSize().y / 2.})) {
        std::shared_ptr<Gui> gui = std::make_shared<Gui>(
            t.character.texture, name + std::to_string(i++));
        gui->addEffect(GraphicalEffect(applyTemp));
        gui->setColor(color);
        gui->setNegate(true);
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        this->game.addGui(gui);
    }
}

void PlayMode::noteMiss(const std::shared_ptr<Note> &note) {
    if (note->getStatus() != WAITING) return;
    int64_t time = game.getCurrentTimeMicros();
    note->setStatus(MISSED);
    note->kill(time);

    std::string text = "MISS";
    std::string name = "miss_" + std::to_string((size_t)note.get()) + "_";
    int i = 0;
    for (Text t : game.getRenderer().getTextHandler().createText(
             text, "Stupid", 11,
             {-1.8, line->getPosition().y - line->getSize().y / 2.})) {
        std::shared_ptr<Gui> gui = std::make_shared<Gui>(
            t.character.texture, name + std::to_string(i++));
        gui->addEffect(GraphicalEffect(applyTemp));
        gui->setColor({1, 0, 0, 1});
        gui->setNegate(true);
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        game.addGui(gui);
    }
}

Chart PlayMode::getChart() { return chart; }
TrackPartition PlayMode::getPartition() { return partition; }

std::string PlayMode::getSongFolder() { return songFolder; }

void PlayMode::playDrumSound(unsigned char pitch) {
    AudioHandler &audio = game.getAudioHandler();
    switch (pitch) {
        case MidiPercussion::KICK:
            audio.playSound("kick");
            break;
        case MidiPercussion::SNARE:
            audio.playSound("snare");
            break;
        case MidiPercussion::RIDE:
            audio.playSound("ride");
            break;
        case MidiPercussion::CRASH:
            audio.playSound("crash");
            break;
        case MidiPercussion::HAT:
            audio.playSound("hat");
            break;
        case MidiPercussion::TOM_HIGH:
            audio.playSound("tom-high");
            break;
        case MidiPercussion::TOM_MID:
            audio.playSound("tom-mid");
            break;
        case MidiPercussion::TOM_LOW:
            audio.playSound("tom-low");
            break;
        default:
            std::cout << "bruh : " << (unsigned int)pitch << "\n";
    }
}

void PlayMode::playPianoSound(unsigned char pitch) {
    AudioSource &source = game.getAudioHandler().playSound("piano");
    float p = std::pow(1. + 1. / 12., (pitch - 60));
    source.setPitch(p);
}

void PlayMode::spawnNote(MidiNote note) {}

PlayMode::~PlayMode() {
    if (music) game.getAudioHandler().removeSound(*this->music);
    if (this->mod) {
        mod.reset();
        Renderer &renderer = game.getRenderer();
        renderer.loadGuiShaders("bin/gui.vert.spv", "bin/def.geom.spv",
                                "bin/def.frag.spv",
                                sizeof(UniformBufferObject));
        renderer.loadFinalShaders("bin/pass.vert.spv", "bin/pass.geom.spv",
                                  "bin/pass.frag.spv",
                                  sizeof(UniformBufferObject));
    }
#ifdef VST
    if (this->plugin) delete this->plugin;
#endif
    game.setLastScore(score);
    game.setReplay(start, replay);
    game.resetScene();
    game.loadMenu("score");
}

void PlayMode::keyCallback(GLFWwindow *win, int key, int scancode, int action,
                           int mods) {
    Synthmania *game = (Synthmania *)glfwGetWindowUserPointer(win);
    Window &window = game->getWindow();
    if (game == NULL || action != GLFW_PRESS) return;
    PlayMode &inst = dynamic_cast<PlayMode &>(game->getGamemode());

    if (key == GLFW_KEY_TAB) {
        inst.autoPlay = !inst.autoPlay;
        for (std::shared_ptr<Gui> &g : inst.autoplayText)
            if (inst.autoPlay)
                g->setColor({.5, .5, 0, 1});
            else
                g->setColor(glm::vec4(0));
        return;
    }
    if (key == GLFW_KEY_RIGHT) {
        game->setTimeMicros(game->getCurrentTimeMicros() + 2000000);
        return;
    }
    if (key == GLFW_KEY_ESCAPE) {
        game->setLastScore(inst.score);
        game->setReplay(inst.start, inst.replay);
        game->setGamemode(NULL);
        game->loadMenu("score");
        return;
    }
    if (inst.autoPlay) return;
    /*int k = 0;
    int keys[] = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_X, GLFW_KEY_D,
                  GLFW_KEY_C, GLFW_KEY_V, GLFW_KEY_G, GLFW_KEY_B,
                  GLFW_KEY_H, GLFW_KEY_N, GLFW_KEY_J, GLFW_KEY_M};
    for (k = 0; k < 12; k++) {
        if (keys[k] == key) break;
    }*/
    for (std::weak_ptr<Note> &note : inst.notes) {
        if (note.expired()) continue;
        std::shared_ptr<Note> n(note);
        // This is ADOFAI for now
        if (  // b->getPitch() == k &&
            n->getStatus() == WAITING &&
            std::abs(n->getTime() - game->getCurrentTimeMicros()) <
                (inst.drum
                     ? DRUM_HIT_WINDOW
                     : HIT_WINDOW)  // && next note not skipped/close?? or
                                    // just set OD to a value that prevents
                                    // the need for notelock idk hmm
        ) {
            inst.noteHit(n);
            if (inst.drum) inst.playDrumSound(n->getPitch());
            break;
        }
    }
}

void PlayMode::onClockAdjust(int64_t t) {
    if (music)
        music->get().setSampleOffset(
            (game.getCurrentTimeMicros() + chart.offset) * 44100 / 1000000.f);
}

void PlayMode::onConfigChange() {
    if (music) music->get().setGain(game.getMusicVolume());
}

size_t PlayMode::updateUBO(void *&ubo) {
    if (mod) return mod->updateUBO(ubo, game.getCurrentTimeMicros());
    return sizeof(UniformBufferObject);
}

void PlayMode::freeUBO(void *&ubo) {
    if (mod) mod->freeUBO(ubo);
}

size_t PlayMode::updateFinalUBO(void *&ubo) {
    if (mod) return mod->updateFinalUBO(ubo, game.getCurrentTimeMicros());
    return sizeof(UniformBufferObject);
}

void PlayMode::freeFinalUBO(void *&ubo) {
    if (mod) mod->freeFinalUBO(ubo);
}

void PlayMode::onSpawn(std::shared_ptr<Gui> &g) {
    if (mod) mod->onSpawn(g);
}