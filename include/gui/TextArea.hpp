#pragma once

class TextArea;

#include "MenuElement.hpp"
#include "TextHandler.hpp"

bool truePredicate(std::wstring cur, int pos, unsigned int text);

bool integerPredicate(std::wstring cur, int pos, unsigned int text);

typedef bool (*TextPredicate)(std::wstring, int, unsigned int);

class TextArea : public MenuElement {
   public:
    TextArea(ImageView* texture, std::string name, TextHandler* handler,
             std::string fontName, int chars, int maxChars,
             unsigned long cursor, float textSize,
             TextPredicate textPredicate = truePredicate);

    virtual void onKeyPressed(int key, int scancode, int mods);

    virtual void onTyped(unsigned int character);

    void showCursor(bool show = true);

    virtual bool update(int64_t time);

    std::wstring getText();
    void setText(std::wstring text);
    void setTextStr(std::string text);

    std::vector<std::shared_ptr<Gui>> getGuis();

    void recalculateText();
    void recalculateCursor();

   private:
    TextHandler* handler;
    std::wstring text;
    std::string fontName;
    std::vector<std::shared_ptr<Gui>> textContents;
    std::shared_ptr<Gui> cursor;
    Text cursorChar;
    int cursorPos, pos, shown, maxChars, blinkTimer;
    float textSize;
    bool cursorVisible;
    TextPredicate predicate;
};