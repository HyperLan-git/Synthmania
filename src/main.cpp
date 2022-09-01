#include "Synthmania.hpp"

// TODO clean includes because it might or might not currently be complete chaos
// TODO put consts everywhere relevant
int main(int argc, char **argv) {
    std::string skin = "resources/textures";
    if (argc > 1 && argv[1][0] != '\0') skin = std::string(argv[1]);
    Synthmania instance(skin);
    instance.setWindow(new Window(1920, 1080, "Synthmania", false));
    instance.setRenderer(new Renderer(&instance, instance.getWindow()));
    instance.init();
    instance.loadMenu("main");
    instance.run();

    return 0;
}