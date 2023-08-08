#include "Synthmania.hpp"

// FIXME replace pointers with unique or shared_ptrs I have just had enough
// FIXME bruh pointers to handles are unnecessary AF you're such a bad
// programmer for not checking types
// TODO clean includes because it might or might not currently be complete chaos
// TODO put consts everywhere relevant
// TODO put inlines everywhere relevant
// TODO get device from other arguments when possible
// TODO this-> ...
// TODO have any object destroy everything that needs it to exist automatically
// XXX do the same for moving and maybe even copying?
// XXX namespace shenanigans
// XXX explicit constructors :crying:
// XXX checking every return value of vulkan functions...
int main(int argc, char **argv) {
    std::string skin = "resources/textures", config = "bin/config.json";
    if (argc > 1 && argv[1][0] != '\0') skin = std::string(argv[1]);
    Synthmania instance(skin, config);

    instance.setWindow(
        new Window(1920, 1080, "Synthmania", instance.isFullscreen()));
    instance.setRenderer(new Renderer(&instance, instance.getWindow()));
    instance.init();
    instance.loadMenu("main");
    instance.run();

    return 0;
}