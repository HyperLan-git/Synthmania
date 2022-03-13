#include "Renderer.hpp"


int main() {
    Renderer *r = new Renderer();

    try {
        r->initWindow();
        r->initVulkan();
        r->mainLoop();
        r->cleanup();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
