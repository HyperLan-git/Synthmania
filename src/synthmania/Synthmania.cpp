#include "Synthmania.hpp"

Synthmania::Synthmania() {
    handler = new MidiHandler();
    window = new Window(1920, 1080, "Synthmania");
    renderer = new Renderer(this, window);
    std::vector<ImageView *> textures = renderer->getTextures();
    this->startTime = 2760000;
    const char *path = "resources/songs/ETEA/ETEA.MID";
    partition = handler->readMidi(path);
    audio = new AudioHandler();
    // Entity *e = new Entity(model, getTextureByName(textures, "room"), "Bob");
    // entities.push_back(e);
    addGui(new Gui(getTextureByName(textures, "partition"), "bg"));
    addGui(new Gui(getTextureByName(textures, "partition"), "partition"));
    addGui(new Gui(getTextureByName(textures, "sol_key"), "key"));
    guis[0]->setSize({10, 30});
    guis[1]->setSize({10, 1});
    guis[2]->setPosition({-1.7f, 0.1f});
    guis[2]->setSize({0.8f, 0.8f});
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
}

void Synthmania::run() {
    while (!window->shouldClose()) {
        glfwPollEvents();
        update();
        renderer->render();
        audio->update();
    }
}

int64_t Synthmania::getCurrentTimeMillis() {
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
            for (Note *note : notes) {
                if (note->getPitch() == m.data1 &&
                    std::abs(note->getTime() - time_from_start) < HIT_WINDOW) {
                    note->setStatus(HIT);
                    break;
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