#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdlib.h>

#include <iostream>
#include <stdexcept>

#include "MidiHandler.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

int main(void) {
    try {
        MidiHandler *handler = new MidiHandler();
        Window *window = new Window(800, 600, "Synthmania");
        Renderer *renderer = new Renderer(window);

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