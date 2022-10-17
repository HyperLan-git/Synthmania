#include "Synthmania.hpp"

// FIXME replace pointers with unique or shared_ptrs I have just had enough
// TODO clean includes because it might or might not currently be complete chaos
// TODO put consts everywhere relevant
// TODO put inlines everywhere relevant
int main(int argc, char **argv) {
    std::string skin = "resources/textures", config = "bin/config.json";
    if (argc > 1 && argv[1][0] != '\0') skin = std::string(argv[1]);
    Synthmania instance(skin, config);
    instance.setWindow(new Window(1920, 1080, "Synthmania", false));
    instance.setRenderer(new Renderer(&instance, instance.getWindow()));
    instance.init();
    instance.loadMenu("main");
    instance.run();

    return 0;
}