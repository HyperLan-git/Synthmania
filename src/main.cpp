#include <test.hpp>

#include "Synthmania.hpp"

int main(int argc, char **argv) {
    printf("%d\nahaha\n", yolo());
    std::string path = "resources/songs/ETEA";
    std::string skin = "resources/textures";
    if (argc > 1 && argv[1][0] != '\0') path = std::string(argv[1]);
    Synthmania instance(path, skin);
    instance.run();

    return 0;
}