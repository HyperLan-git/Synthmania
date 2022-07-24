#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Gui.hpp"
#include "Renderer.hpp"

class Test : public Game {
   public:
    Test(int width, int height, const char* title) : Game() {
        this->window = new Window(width, height, title);
        this->renderer = new Renderer(this, window);
    }

    void init() {
        Gui* g = new Gui(getTextureByName(renderer->getTextures(), "missing"),
                         "back");
        g->setSize({5.f, 5.f});
        addGui(g);
        int i = 0;
        for (Text t :
             renderer->createText("Hello", "Stupid", 55, glm::vec2({-1, 0}))) {
            std::string name = "hi";
            name.append(std::to_string(i++));
            Gui* gui = new Gui(t.character.texture, name.c_str());
            gui->setColor(glm::vec4({1, 0, 0, 1}));
            gui->setPosition(t.pos);
            gui->setSize(t.size);
            addGui(gui);
        }
    }

    void update() {}

    ~Test() {}

   private:
};

int main() {
    Test* game = new Test(1920, 1080, "GraphicsTest");
    game->run();
    delete game;
    return 0;
}