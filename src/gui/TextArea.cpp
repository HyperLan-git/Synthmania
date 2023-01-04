#include "TextArea.hpp"

bool truePredicate(std::wstring cur, int pos, unsigned int text) {
    return true;
}

bool integerPredicate(std::wstring cur, int pos, unsigned int text) {
    return text >= '0' && text <= '9';
}

TextArea::TextArea(ImageView* texture, std::string name, TextHandler* handler,
                   std::string fontName, int chars, int maxChars,
                   unsigned long cursor, float textSize,
                   TextPredicate textPredicate)
    : MenuElement(texture, name) {
    this->handler = handler;
    this->pos = 0;
    this->cursorPos = 0;
    this->shown = chars;
    this->maxChars = maxChars;
    this->fontName = fontName;
    this->textSize = textSize;
    this->cursorVisible = true;
    this->predicate = textPredicate;
    text.reserve(maxChars);
    for (int i = 0; i < chars; i++) {
        std::shared_ptr<Gui> g = std::make_shared<Gui>(texture, name);
        g->setSize({0, 0});
        this->textContents.push_back(g);
    }
    wchar_t str[2] = {(wchar_t)cursor, 0};
    this->cursorChar = handler->createText_w(std::wstring(str), fontName,
                                             textSize * .9, {0, 0})[0];
    this->cursor =
        std::make_shared<Gui>(cursorChar.character.texture, name + "_cursor");
    this->cursor->setSize(cursorChar.size);
}

void TextArea::onKeyPressed(int key, int scancode, int mods) {
    switch (key) {
        case GLFW_KEY_V:
            if (!(mods & GLFW_MOD_CONTROL)) break;
            break;
        case GLFW_KEY_LEFT:
            if (pos + cursorPos <= 0) break;
            if (mods & GLFW_MOD_CONTROL) {
                pos = 0;
                cursorPos = 0;
                recalculateText();
                recalculateCursor();
                break;
            }
            if (cursorPos > 0) {
                cursorPos--;
                recalculateCursor();
            } else {
                pos--;
                recalculateText();
                recalculateCursor();
            }
            break;
        case GLFW_KEY_RIGHT:
            if (cursorPos + pos >= text.size()) break;
            if (cursorPos == shown) {
                pos++;
                recalculateText();
            } else {
                cursorPos++;
            }
            if (mods & GLFW_MOD_CONTROL) {
                if (text.size() > shown) {
                    pos = text.size() - shown;
                    cursorPos = shown;
                    recalculateText();
                } else
                    cursorPos = text.size();
                recalculateCursor();
                break;
            }
            recalculateCursor();
            break;
        case GLFW_KEY_BACKSPACE:
            if (cursorPos + pos == 0) break;
            std::wstring str = text.substr(0, cursorPos + pos - 1) +
                               text.substr(cursorPos + pos);
            str.reserve(maxChars);
            this->text = str;
            if (cursorPos == 0)
                pos--;
            else
                cursorPos--;
            recalculateText();
            recalculateCursor();
            break;
    }
}

void TextArea::onTyped(unsigned int character) {
    if (!predicate(text, pos + cursorPos, character) || text.size() >= maxChars)
        return;
    if (pos + cursorPos >= this->text.size()) {
        wchar_t str[2] = {(wchar_t)character, 0};
        text.append(str);
    } else {
        wchar_t str[2] = {(wchar_t)character, 0};
        std::wstring add = str;
        int pos = this->pos + cursorPos;
        text = text.substr(0, pos) + add + text.substr(pos);
    }
    if (cursorPos < shown)
        cursorPos++;
    else
        pos++;
    recalculateText();
    recalculateCursor();
}

void TextArea::showCursor(bool show) {
    this->cursorVisible = show;
    this->cursor->setColor({1, 1, 1, show ? 1 : 0});
}

std::wstring TextArea::getText() { return text; }

void TextArea::setText(std::wstring text) { this->text = text; }

void TextArea::setTextStr(std::string text) {
    size_t l = text.size();
    if (l > this->maxChars) l = this->maxChars;
    wchar_t contents[l + 1] = {0};
    for (int i = 0; i < l; i++) contents[i] = text[i];
    this->text = std::wstring(contents);
}

std::vector<std::shared_ptr<Gui>> TextArea::getGuis() {
    std::vector<std::shared_ptr<Gui>> result = textContents;
    result.push_back(cursor);
    return result;
}

bool TextArea::update(int64_t time) {
    int64_t dt = 1000000;
    if (selected)
        showCursor((time / dt) % 2 == 0);
    else
        showCursor(false);
    return false;
}

void TextArea::recalculateText() {
    std::wstring sub = text.substr(pos);
    int i = 0;
    for (Text t :
         handler->createText_w(sub, fontName, textSize,
                               {position.x - size.x * .5 + cursor->getSize().x,
                                position.y + size.y * .25})) {
        std::shared_ptr<Gui> g = this->textContents[i++];
        g->setTexture(t.character.texture);
        g->setPosition(t.pos);
        g->setSize(t.size);
        if (i >= textContents.size()) return;
    }
    for (; i < textContents.size(); i++) this->textContents[i]->setSize({0, 0});
}

void TextArea::recalculateCursor() {
    glm::vec2 pos = cursorChar.pos;
    pos.y += this->position.y + size.y * .25;
    if (cursorPos == 0) {
        pos.x += getPosition().x - getSize().x / 2 + cursorChar.size.x / 2;
        this->cursor->setPosition(pos);
        return;
    }
    size_t cPos = cursorPos;
    bool end = (cPos >= (this->text.size() - this->pos) ||
                cPos >= this->textContents.size()) &&
               cPos > 0;
    if (end) cPos--;
    std::shared_ptr<Gui> g = this->textContents[cPos];
    if (end) pos.x += g->getSize().x;
    pos.x += g->getPosition().x + g->getGraphicalPosition().x - g->getSize().x +
             cursorChar.size.x / 4;
    this->cursor->setPosition(pos);
    this->cursor->setColor({0, 0, 0, cursorVisible ? 1 : 0});
}