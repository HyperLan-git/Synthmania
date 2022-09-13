#include "Selector.hpp"

Selector::Selector(ImageView* texture, ImageView* selectTexture, Game* game,
                   std::string name, std::vector<std::string> elements,
                   double textSize, std::string font, int textLimit)
    : MenuElement(texture, name) {
    this->textLimit = textLimit;
    this->opened = false;
    this->current = -1;
    for (std::string e : elements) {
        std::vector<std::pair<Text, Gui*>> vec;
        if (e.length() > textLimit) e = e.substr(0, textLimit - 2) + "..";
        for (Text t :
             game->getRenderer()->createText(e, font, textSize, {0, 0})) {
            Gui* g = new Gui(t.character.texture, name + "_text");
            g->setColor({1, 1, 1, 0});
            g->setSize(t.size);
            vec.push_back(std::make_pair(t, g));
        }
        strings.push_back(vec);
        Gui* g = new Gui(selectTexture, name + "_button");
        g->setColor({1, 1, 1, 0});
        buttons.push_back(g);
    }
}

int Selector::getSelected() { return current; }

bool Selector::isInside(glm::vec2 position) {
    if (!opened) return MenuElement::isInside(position);
    return position.x <= (this->position.x + this->size.x / 2) &&
           position.x >= (this->position.x - this->size.x / 2) &&
           position.y <=
               (this->position.y + this->size.y * (buttons.size() + .5)) &&
           position.y >= (this->position.y - this->size.y / 2);
}

void Selector::onClicked(glm::vec2 pos) {
    opened = !opened;
    glm::vec4 color;
    if (opened)
        color = {1, 1, 1, 1};
    else {
        color = {1, 1, 1, 0};
        int place =
            (pos.y + this->size.y / 2 - this->position.y) / this->size.y;
        std::cout << "pos:" << (pos.y + this->size.y / 2 - this->position.y)
                  << "\nclicked : " << place << std::endl;
        if (place > 0) {
            if (place >= current) place--;
            current = place;
            recalculatePositions();
        }
    }

    for (Gui* g : buttons) g->setColor(color);
    int elem = 0;
    for (std::vector<std::pair<Text, Gui*>> elements : strings) {
        if (elem++ != current)
            for (std::pair<Text, Gui*> c : elements) c.second->setColor(color);
        else
            for (std::pair<Text, Gui*> c : elements)
                c.second->setColor({1, 1, 1, 1});
    }
}

std::vector<Gui*> Selector::getGuis() {
    std::vector<Gui*> result = buttons;
    for (std::vector<std::pair<Text, Gui*>> elements : strings)
        for (std::pair<Text, Gui*> c : elements) result.push_back(c.second);
    return result;
}

void Selector::recalculatePositions() {
    int i = 0;
    glm::vec2 gpos = this->position;
    for (Gui* g : buttons) {
        gpos.y += this->size.y;
        g->setPosition(gpos);
        g->setSize(this->size);
    }
    for (std::vector<std::pair<Text, Gui*>> elements : strings) {
        int place = i + 1;
        if (i == current) place = 0;

        for (std::pair<Text, Gui*> c : elements) {
            glm::vec2 pos = c.first.pos;
            pos.y += this->position.y + this->size.y * place;
            pos.x += this->position.x - this->size.x * .45;
            c.second->setPosition(pos);
        }

        i++;
    }
}