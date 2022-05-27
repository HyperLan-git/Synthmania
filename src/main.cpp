#include "Synthmania.hpp"

int main(int argc, char **argv) {
    Synthmania instance = Synthmania("resources/songs/ETEA");
    instance.run();

    return 0;
}