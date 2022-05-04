#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdlib.h>

#include <iostream>
#include <stdexcept>

#include "MidiHandler.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

int main(int argc, char **argv) {
    try {
        MidiHandler *handler = new MidiHandler();
        Window *window = new Window(1920, 1080, "Synthmania");
        Renderer *renderer = new Renderer(window);
        const char *path = "resources/bruh.MID";
        if (argc > 1) path = argv[1];
        auto result = handler->readMidi(path);
        for (MidiNote note : result) {
            renderer->addGui(new Note("E", note.timestamp / 1000000.f,
                                      note.note, 0.25f,
                                      renderer->getTextures()));
        }
        while (!window->shouldClose()) {
            glfwPollEvents();
            renderer->render();
        }

        delete handler;

        delete renderer;
        delete window;

        glfwTerminate();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}