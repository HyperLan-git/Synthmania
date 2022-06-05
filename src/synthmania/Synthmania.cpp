#include "Synthmania.hpp"

Synthmania::Synthmania(std::string song, std::string skin) {
    std::string json = song;
    json.append("/sdata.json");
    Chart c = readChart(json.c_str());
    Diff d = c.diffs[0];
    handler = new MidiHandler();
    window = new Window(1920, 1080, "Synthmania");
    renderer = new Renderer(this, window);
    textures = readTextures(std::string(skin).append("/skin.json"));
    for (auto &elem : textures)
        elem.second = std::string(skin).append("/").append(elem.second);
    renderer->loadTextures(textures);
    this->startTime = c.offset;
    std::string path = song;
    path.append("/");
    path.append(d.midi);
    partition = handler->readMidi(path.c_str());
    audio = new AudioHandler();
    // Entity *e = new Entity(model, getTextureByName(textures, "room"), "Bob");
    // entities.push_back(e);
    std::vector<ImageView *> textures = renderer->getTextures();
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
    bar->setPosition({-1.4f, 0.f});
    bar->setSize({0.25f, 1.f});
    addGui(bar);
    for (MidiNote note : partition.notes) {
        char name[255] = "Note_";
        const char *hash = std::to_string(std::hash<MidiNote>()(note)).c_str();
        strcat(name, hash);
        Note *n = new Note(name, note.timestamp, note.note,
                           note.length / partition.MPQ / 8., textures);
        notes.push_back(n);
        addGui(n);
        if (!isFromCMajor(note.note)) {
            strcpy(name, "Sharp_");
            strcat(name, hash);
            ParentedGui *sharp =
                new ParentedGui(getTextureByName(textures, "sharp"), name, n);
            sharp->setPosition({-0.25, 0.15});
            sharp->setSize({0.25, 0.25});
            addGui(sharp);
        }
    }
    music = NULL;
    if (c.audio.compare("None") != 0) {
        std::string wav = song;
        wav.append("/");
        wav.append(c.audio);
        AudioBuffer *buffer = new AudioBuffer(wav.c_str());

        audio->addSound("song", buffer);
        music = audio->playSound("song");
    }
}

std::map<std::string, std::string> Synthmania::getTextures() {
    return textures;
}

std::map<std::string, std::string> Synthmania::readTextures(std::string skin) {
    std::map<std::string, std::string> result;
    tree *t = readJson(skin.c_str());
    readTree(*t, result, "");
    delete t;
    return result;
}

Window *Synthmania::getWindow() { return this->window; }

void Synthmania::run() {
    window->setWindowUserPointer(this);
    window->setKeycallback(
        [](GLFWwindow *win, int key, int scancode, int action, int mods) {
            Synthmania *game = (Synthmania *)glfwGetWindowUserPointer(win);
            Window *window = game->getWindow();
            if (game == NULL || action != GLFW_PRESS) return;
            int k = 0;
            switch (key) {
                case GLFW_KEY_W:
                    k = 60;
                    break;
                case GLFW_KEY_X:
                    k = 62;
                    break;
                case GLFW_KEY_C:
                    k = 64;
                    break;
                case GLFW_KEY_V:
                    k = 65;
                    break;
                case GLFW_KEY_B:
                    k = 67;
                    break;
                case GLFW_KEY_N:
                    k = 69;
                    break;
                case GLFW_KEY_M:
                    k = 71;
                    break;
                default:
                    std::cout << "key = " << key << std::endl;
            }

            for (Note *note : game->notes) {
                // This is osu for now
                if (  // note->getPitch() == k &&
                    note->getStatus() == WAITING &&
                    std::abs(note->getTime() - game->getCurrentTimeMillis()) <
                        HIT_WINDOW) {
                    uint64_t time = game->getCurrentTimeMillis();
                    // a -= (note->getTime() - game->getCurrentTime()) /
                    // ++i; std::cout << "avg = " << std::to_string(a) <<
                    // std::endl;
                    Precision *prec = new Precision(
                        getTextureByName(game->getRenderer()->getTextures(),
                                         "precision_tick"),
                        "tick", time, time - note->getTime());
                    prec->setSize({0.1f, 0.4f});
                    prec->setPosition({0, 0.9f});
                    game->addGui(prec);
                    note->setStatus(HIT);
                    note->kill(time);
                    break;
                }
            }
        });
    while (!window->shouldClose()) {
        glfwPollEvents();
        update();
        audio->update();
        renderer->render();
    }
}

Renderer *Synthmania::getRenderer() { return renderer; }

int64_t Synthmania::getCurrentTimeMillis() {
    if (music != NULL) {
        long long j = music->getSampleOffset() * 1000000.;
        return j / 44100 - this->startTime;
    }
    auto currentTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<int64_t, std::chrono::nanoseconds::period>(
               currentTime - begTime)
                   .count() /
               1000 -
           this->startTime;
}

void Synthmania::update() {
    int64_t time_from_start = getCurrentTimeMillis();
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
}

void Synthmania::addGui(Gui *gui) {
    if (guis.empty()) {
        gui->setZ(0.9999f);
    } else {
        gui->setZ((*--guis.end())->getPosition().z - 0.0001f);
    }
    this->guis.push_back(gui);
}

void Synthmania::addEntity(Entity *entity) { entities.push_back(entity); }

std::vector<Entity *> Synthmania::getEntities() { return entities; }
std::vector<Gui *> Synthmania::getGuis() { return guis; }

Synthmania::~Synthmania() {
    for (Entity *e : entities) delete e;
    for (Gui *g : guis) delete g;

    delete audio;

    delete handler;

    delete renderer;
    delete window;

    glfwTerminate();
}