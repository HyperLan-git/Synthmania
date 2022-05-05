#define GLFW_INCLUDE_VULKAN
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

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
        for (MidiNote note : result.notes) {
            char name[255] = "Note_";
            strcat(name, std::to_string(std::hash<MidiNote>()(note)).c_str());
            Note *n = new Note(name, note.timestamp / 1000000.f, note.note,
                               0.25f, renderer->getTextures());
            std::cout << n->getName() << "\n";
            renderer->addGui(n);
        }
        Judgement *bar =
            new Judgement("judgement", renderer->getTextures(), result);
        bar->setPosition({-1.4f, 0.f});
        bar->setSize({0.25f, 1.f});
        renderer->addGui(bar);
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