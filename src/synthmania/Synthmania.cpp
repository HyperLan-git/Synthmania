#include "Synthmania.hpp"
// TODO fix the white half note being slightly misplaced to the right

Synthmania::Synthmania(std::string song, std::string skin) {
    this->songFolder = song;
    this->skin = skin;
    handler = new MidiHandler();
    textures = readTextures(std::string(skin).append("/skin.json"));
    for (auto &elem : textures)
        elem.second = std::string(skin).append("/").append(elem.second);
    audio = new AudioHandler();
    if (!autoPlay) keyFunction = Synthmania::keyCallback;
}

void Synthmania::init() {
    std::string json = songFolder;
    json.append("/sdata.json");
    chart = readChart(json.c_str());
    diff = chart.diffs[0];
    this->startTime = chart.offset;
    if (chart.animation.compare("None") != 0) {
        void *shared = loadShared(songFolder + "/" + chart.animation);
        char *e = dlerror();
        if (e != NULL) {
            std::cout << "Error while loading anim ! " << e << "\n";
        } else {
            GraphicalEffectHandler *(*f)(Synthmania *) =
                (GraphicalEffectHandler * (*)(Synthmania *))
                    getFunction(shared, "getEffectHandler");
            char *e = dlerror();
            if (e != NULL)
                std::cout << "Error while loading anim ! " << e << "\n";
            else
                this->mod = f(this);  // Shit
        }
    }
    std::string path = songFolder;
    path.append("/");
    path.append(diff.midi);
    partition = handler->readMidi(path.c_str());
    std::string pdata = songFolder;
    pdata.append("/");
    pdata.append(chart.plugindata);
    if (chart.plugindata.compare("None") == 0) pdata = "None";
    plugin = new AudioPluginHandler("./plugins/Vital.vst3", audio, pdata);
    Model *model =
        new Model("resources/models/room.obj", renderer->getPhysicalDevice(),
                  renderer->getDevice());
    std::vector<ImageView *> textures = renderer->getTextures();
    Entity *e = new Entity(model, getTextureByName(textures, "room"), "Bob");
    entities.push_back(e);
    Gui *part = new Gui(getTextureByName(textures, "partition"), "partition"),
        *bg = new Gui(getTextureByName(textures, "partition"), "bg"),
        *key = new Gui(getTextureByName(textures, "sol_key"), "key"),
        *precision =
            new Gui(getTextureByName(textures, "precision"), "precision");
    addGui(bg);
    addGui(part);
    addGui(key);
    bg->setSize({10, 30});
    part->setSize({10, 1});
    key->setPosition({-1.7f, 0.1f});
    key->setSize({0.8f, 0.8f});
    precision->setSize({1.5f, 0.5f});
    precision->setPosition({0, 0.9f});
    Judgement *bar = new Judgement("judgement", textures, partition);
    line = bar;
    bar->setPosition({-1.4f, bar->getPosition().y});
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
            if (cutDown.size() > 1 && cutDown[i] == cutDown[i - 1] / 2.) {
                firstDots = i;
            }
        for (int i = 0; i < firstDots; i++) {
            cutDown[1] += cutDown[0];
            cutDown.erase(cutDown.begin());
        }
        Note *n = new Note(name.c_str(), note.timestamp, note.note,
                           totalDuration, cutDown[0], partition.MPQ, textures);
        notes.push_back(n);
        int diff = getDifferenceFromC4(note.note);
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

        if (!isFromCMajor(note.note)) {
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
                getTextureForNote(textures, note.note, d, Key::SOL),
                name2.c_str(), n);
            glm::vec2 temp = getSizeAndLocForNote(d);
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
    sortGuis(tmp, cmpGuisByTexture);
    sortGuis(tempNotes, cmpGuisByTexture);
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
        music->setGain(.5f);
    }
    std::string text = chart.name;
    text.append(" by ");
    text.append(chart.artist);

    for (Gui *g : printShadowedString(text, renderer, "title_", "Stupid", 11,
                                      {-2, -.9}, {.2, .2, 1, 1}))
        addGui(g);

    /*for (Gui *g : printShakingString("ANGERY", renderer, "scary_", "Stupid",
       22, {0, 0}, .001, {1, 0, 0, 1})) addGui(g);*/
    // Needs to be above everything else
    addGui(precision);
}

void Synthmania::keyCallback(GLFWwindow *win, int key, int scancode, int action,
                             int mods) {
    Synthmania *game = (Synthmania *)glfwGetWindowUserPointer(win);
    Window *window = game->getWindow();
    if (game == NULL || action != GLFW_PRESS) return;
    if (key == GLFW_KEY_RIGHT) {
        game->setTimeMicros(game->getCurrentTimeMicros() + 2000000);
        return;
    }
    int k = 0;
    int keys[] = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_X, GLFW_KEY_D,
                  GLFW_KEY_C, GLFW_KEY_V, GLFW_KEY_G, GLFW_KEY_B,
                  GLFW_KEY_H, GLFW_KEY_N, GLFW_KEY_J, GLFW_KEY_M};
    for (k = 0; k < 12; k++) {
        if (keys[k] == key) break;
    }
    for (Note *note : game->notes) {
        // This is osu for now
        if (  // note->getPitch() == k &&
            note->getStatus() == WAITING &&
            std::abs(note->getTime() - game->getCurrentTimeMicros()) <
                HIT_WINDOW  // && next note not skipped/close?? or just
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
    plugin->playNote(note->getPitch(), 90,
                     time + note->getTotalDuration() - delta);
    note->setStatus(HIT);
    note->kill(time + note->getTotalDuration());

    std::string text = "Good!";
    int i = 0;
    for (Text t :
         renderer->createText(text, "Stupid", 11,
                              {-2, (double)(-.25 + line->getPosition().y +
                                            line->getSize().y / 2.)})) {
        std::string name = "hit_";
        name.append(std::to_string((size_t)note));
        name.append("_");
        name.append(std::to_string(i++));
        Gui *gui = new Gui(t.character.texture, name.c_str());
        gui->addEffect(new GraphicalEffect(applyTemp));
        gui->setColor({.3, 1, .3, 1});
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        addGui(gui);
    }
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
    for (Text t :
         renderer->createText(text, "Stupid", 11,
                              {-2, (double)(-.25 + line->getPosition().y +
                                            line->getSize().y / 2.)})) {
        std::string name = "miss_";
        name.append(std::to_string((size_t)note));
        name.append("_");
        name.append(std::to_string(i++));
        Gui *gui = new Gui(t.character.texture, name.c_str());
        gui->addEffect(new GraphicalEffect(applyTemp));
        gui->setColor({1, 0, 0, 1});
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        addGui(gui);
    }
}

void Synthmania::setTimeMicros(int64_t time) {
    Game::setTimeMicros(time);
    if (music != NULL)
        music->setSampleOffset((getCurrentTimeMicros() + startTime) * 44100. /
                               1000000.f);
}

#define instanceOf(type, ptr) (dynamic_cast<type *>(ptr) != NULL)

void Synthmania::update() {
    int64_t time_from_start = getCurrentTimeMicros();
    // std::cout << std::dec << time_from_start << " ";
    if (autoPlay) {
        for (Note *note : notes) {
            if (note->getTime() <= time_from_start) noteHit(note);
        }
    }
    plugin->update(time_from_start);
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
                for (Note *note : notes) {
                    if (note->getStatus() == WAITING &&
                        note->getPitch() == m.data1 &&
                        std::abs(note->getTime() - time_from_start) <
                            HIT_WINDOW) {
                        noteHit(note);
                        got_one = true;
                        break;
                    }
                }
                if (!got_one) {
                    for (Note *note : notes) {
                        if (note->getStatus() == WAITING &&
                            std::abs(note->getTime() - time_from_start) <
                                HIT_WINDOW) {
                            noteMiss(note);
                        }
                    }
                }
            }
            m = handler->getMessage();
        }
    }
    if (audio != NULL) {  // audio latency study
        long long j = music->getSampleOffset() * 1000000.;
        int64_t res = j / (uint64_t)44100 - this->startTime;
        int64_t a = getCurrentTimeMicros();
        int64_t b = res - a;
        // std::cout << b << std::dec << " ";
        // If music gets more than 10ms off reset it
        // Most people can play with 10ms off right? (I'm sorry rythm gamers)
        if (b > 10000) setTimeMicros(a);
    }
    // thrd_yield();
}

size_t Synthmania::updateUBO(void *&ubo) {
    if (mod != NULL) return mod->updateUBO(ubo, getCurrentTimeMicros());
    return sizeof(UniformBufferObject);
}

Chart Synthmania::getChart() { return chart; }

TrackPartition Synthmania::getPartition() { return partition; }

AudioPluginHandler *Synthmania::getPluginHandler() { return plugin; }

Synthmania::~Synthmania() {
    if (mod != NULL) delete mod;
    delete plugin;
    delete audio;

    delete handler;
}

std::vector<Gui *> printString(std::string text, Renderer *renderer,
                               std::string entityNames, std::string font,
                               double size, glm::vec2 pos, glm::vec4 color) {
    std::vector<Gui *> result;
    int i = 0;
    for (Text t : renderer->createText(text, font, size, pos)) {
        std::string name = entityNames;
        name.append(std::to_string(i++));
        Gui *gui = new Gui(t.character.texture, name.c_str());
        gui->setColor(color);
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        result.push_back(gui);
    }
    return result;
}

std::vector<Gui *> printShadowedString(std::string text, Renderer *renderer,
                                       std::string entityNames,
                                       std::string font, double size,
                                       glm::vec2 pos, glm::vec4 color) {
    std::vector<Gui *> result, result2;
    glm::vec2 shadowPos = pos;
    shadowPos += glm::vec2({.0005 * size, .0005 * size});
    std::string shadowName = entityNames;
    shadowName.append("shadow_");
    for (Gui *g : printString(text, renderer, shadowName, font, size, shadowPos,
                              {0, 0, 0, .7})) {
        glm::vec2 sz = g->getSize();
        sz *= 1.05;
        g->setSize(sz);
        result.push_back(g);
    }
    for (Gui *g :
         printString(text, renderer, entityNames, font, size, pos, color))
        result.push_back(g);
    for (int i = 0; i < result.size() / 2; i++) {
        result2.push_back(result[i]);
        result2.push_back(result[i + result.size() / 2]);
    }

    return result2;
}

std::vector<Gui *> printShakingString(std::string text, Renderer *renderer,
                                      std::string entityNames, std::string font,
                                      double size, glm::vec2 pos, float shake,
                                      glm::vec4 color) {
    std::vector<Gui *> result;
    int i = 0;
    for (Text t : renderer->createText(text, font, size, pos)) {
        std::string name = entityNames;
        name.append(std::to_string(i++));
        Gui *gui = new Gui(t.character.texture, name.c_str());
        gui->addEffect(new GraphicalEffect(applyShaking,
                                           new float[]{shake * (float)size}));
        gui->setColor(color);
        gui->setPosition(t.pos);
        gui->setSize(t.size);
        result.push_back(gui);
    }
    return result;
}