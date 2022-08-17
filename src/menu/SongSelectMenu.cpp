#include "SongSelectMenu.hpp"

#include <boost/filesystem.hpp>

SongSelectMenu::SongSelectMenu(Game *g, std::string folder) : Menu(g) {
    for (Text t : g->getRenderer()->createText("Back", "Stupid", 12,
                                               glm::vec2({-1.8, .9}))) {
        Gui *g = new Gui(t.character.texture, "Back_arrow");
        g->setPosition(t.pos);
        g->setSize(t.size);
        g->setColor(glm::vec4(0, 0, 0, 1));
        guis.push_back(g);
    }
    this->folder = folder;
    if (!boost::filesystem::exists(folder)) return;
    boost::filesystem::directory_iterator end;
    float pos = 1;
    for (boost::filesystem::directory_iterator iter(folder); iter != end;
         iter++) {
        if (boost::filesystem::is_directory(iter->status())) {
            songs.emplace_back(iter->path().leaf().c_str());
            std::string &name = songs[songs.size() - 1];
            Button *but = new Button(
                getTextureByName(g->getRenderer()->getTextures(), "button"),
                getTextureByName(g->getRenderer()->getTextures(),
                                 "button-pressed"),
                name.c_str(), {0, pos}, {1, .5});
            this->buttons.push_back(but);
            for (Text t : g->getRenderer()->createText(name, "Stupid", 10,
                                                       glm::vec2({-.45, 0}))) {
                ParentedGui *g =
                    new ParentedGui(t.character.texture, "Song_name", but);
                g->setPosition(t.pos);
                g->setSize(t.size);
                g->setColor(glm::vec4(0, 0, 0, 1));
                guis.push_back(g);
            }
            pos -= 0.5;
        }
    }
}

void SongSelectMenu::show() { Menu::show(); }

void SongSelectMenu::onPressed(Button *b) {
    Synthmania *s = dynamic_cast<Synthmania *>(game);
    if (s != NULL) {
        s->playSound("click");
        s->resetScene();
        s->loadSong(folder + "/" + b->getName());
        s->setTimeMicros(-s->getChart().offset);
    }
}

void SongSelectMenu::update(int64_t time) {}

SongSelectMenu::~SongSelectMenu() {}