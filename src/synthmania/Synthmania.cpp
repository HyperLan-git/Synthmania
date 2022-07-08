#include "Synthmania.hpp"

Synthmania::Synthmania(std::string song, std::string skin)
    : Game(1920, 1080, "Synthmania") {
    this->songFolder = song;
    this->skin = skin;
    handler = new MidiHandler();
    textures = readTextures(std::string(skin).append("/skin.json"));
    for (auto &elem : textures)
        elem.second = std::string(skin).append("/").append(elem.second);
    audio = new AudioHandler();
    keyFunction = Synthmania::keyCallback;
}

void Synthmania::init() {
    std::string json = songFolder;
    json.append("/sdata.json");
    Chart c = readChart(json.c_str());
    Diff d = c.diffs[0];
    this->startTime = c.offset;
    std::string path = songFolder;
    path.append("/");
    path.append(d.midi);
    partition = handler->readMidi(path.c_str());
    std::string pdata = songFolder;
    pdata.append("/");
    pdata.append(c.plugindata);
    if (c.plugindata.compare("None") == 0) pdata = "None";
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
    addGui(precision);
    bg->setSize({10, 30});
    part->setSize({10, 1});
    key->setPosition({-1.7f, 0.1f});
    key->setSize({0.8f, 0.8f});
    precision->setSize({1.5f, 0.5f});
    precision->setPosition({0, 0.9f});
    Judgement *bar = new Judgement("judgement", textures, partition);
    bar->setPosition({-1.4f, bar->getPosition().y});
    bar->setSize({0.25f, 1.f});
    addGui(bar);
    for (MidiNote note : partition.notes) {
        char name[255] = "Note_";
        const char *hash = std::to_string(std::hash<MidiNote>()(note)).c_str();
        strcat(name, hash);
        Note *n = new Note(name, note.timestamp, note.note,
                           note.length / (long double)partition.MPQ / 4.,
                           partition.MPQ, textures);
        notes.push_back(n);
        int diff = getDifferenceFromC4(note.note);
        if (diff <= 0 || diff >= 12) {
            bool up = diff >= 12;
            for (int i = up ? 12 : 0; (up && i <= diff) || (!up && i >= diff);
                 up ? i += 2 : i -= 2) {
                strcpy(name, "Bar_");
                strcat(name, hash);
                PartitionNotation *readability = new PartitionNotation(
                    name, note.timestamp, (0.5 - 0.083 * i),
                    getTextureByName(textures, "bar"));
                readability->setPosition({0, readability->getPosition().y});
                readability->setSize({0.25, 0.15});
                addGui(readability);
            }
        }
        addGui(n);
        if (!isFromCMajor(note.note)) {
            strcpy(name, "Sharp_");
            strcat(name, hash);
            ParentedGui *sharp =
                new ParentedGui(getTextureByName(textures, "sharp"), name, n);
            sharp->setPosition({-0.25, 0});
            sharp->setSize({0.25, 0.25});
            addGui(sharp);
        }
    }
    music = NULL;
    if (c.audio.compare("None") != 0) {
        std::string wav = songFolder;
        wav.append("/");
        wav.append(c.audio);
        AudioBuffer *buffer = new AudioBuffer(wav.c_str());

        audio->addSound("song", buffer);
        music = audio->playSound("song");
        music->setGain(.5f);
    }
    double pos = -2;
    double conv = 8. * FONT_SIZE;
    std::string text = c.name;
    text.append(" by ");
    text.append(c.artist);
    for (int i = 0; i < text.size(); i++) {
        Character c = renderer->getCharacter("Stupid", (ulong)text[i]);
        std::string name = "text_";
        name.append(std::to_string(i));
        Gui *A = new Gui(c.texture, name.c_str());
        A->setColor({.2, .2, 1, 1});
        A->setPosition(
            {pos + (double)c.width / conv / 2 - c.offsetLeft / conv * 0.7,
             -.9 + (double)c.height / conv / 2 - c.offsetTop / conv * 0.7});
        A->setSize({(double)c.width / conv, (double)c.height / conv});
        addGui(A);
        pos += (c.advance) / conv;
    }
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
            int64_t time = game->getCurrentTimeMicros();
            // a -= (note->getTime() - game->getCurrentTime()) /
            // ++i; std::cout << "avg = " << std::to_string(a) <<
            // std::endl;
            int64_t delta = time - note->getTime();
            Precision *prec = new Precision(
                getTextureByName(game->getRenderer()->getTextures(),
                                 "precision_tick"),
                "tick", time, delta);
            prec->setSize({0.1f, 0.4f});
            prec->setPosition({0, 0.9f});
            game->addGui(prec);
            delta = std::clamp<int64_t>(delta, 0, note->getDuration() / 2);
            game->getPluginHandler()->playNote(
                note->getPitch(), 90, time + note->getDuration() - delta);
            note->setStatus(HIT);
            note->kill(time);
            break;
        }
    }
}

void Synthmania::setTimeMicros(int64_t time) {
    Game::setTimeMicros(time);
    if (music != NULL)
        music->setSampleOffset((getCurrentTimeMicros() + startTime) * 44100. /
                               1000000.f);
}

void Synthmania::update() {
    int64_t time_from_start = getCurrentTimeMicros();
    // std::cout << std::dec << time_from_start << " ";
    plugin->update(time_from_start);
    std::vector<Gui *> toDestroy;
    for (Gui *g : guis)
        if (g->update(time_from_start)) {
            g->setDestroyed();
            toDestroy.push_back(g);
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
    Message m = handler->getMessage();
    while (m.type != libremidi::message_type::INVALID) {
        if (m.type == libremidi::message_type::NOTE_ON) {
            short got_one = false;
            for (Note *note : notes) {
                if (note->getStatus() == WAITING &&
                    note->getPitch() == m.data1 &&
                    std::abs(note->getTime() - time_from_start) < HIT_WINDOW) {
                    Precision *prec =
                        new Precision(getTextureByName(renderer->getTextures(),
                                                       "precision_tick"),
                                      "tick", time_from_start,
                                      time_from_start - note->getTime());
                    prec->setSize({0.1f, 0.4f});
                    prec->setPosition({0, 0.9f});
                    this->addGui(prec);
                    note->setStatus(HIT);
                    note->kill(time_from_start);
                    got_one = true;
                    break;
                }
            }
            if (!got_one) {
                for (Note *note : notes) {
                    if (note->getStatus() == WAITING &&
                        std::abs(note->getTime() - time_from_start) <
                            HIT_WINDOW) {
                        note->setStatus(MISSED);
                        note->kill(time_from_start);
                    }
                }
            }
        }
        m = handler->getMessage();
    }
    if (audio != NULL) {  // audio latency study
        long long j = music->getSampleOffset() * 1000000.;
        int64_t res = j / (uint64_t)44100 - this->startTime;
        int64_t a = getCurrentTimeMicros();
        int64_t b = res - a;
        // std::cout << b << std::dec << " ";
        if (b > 10000) setTimeMicros(a);
    }
    thrd_yield();
}

AudioPluginHandler *Synthmania::getPluginHandler() { return plugin; }

Synthmania::~Synthmania() {
    delete plugin;
    delete audio;

    delete handler;
}