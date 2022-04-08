#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

#include "Renderer.hpp"
#include "Window.hpp"

int main(void) {
    try {
        Window *window = new Window(800, 600, "Vulkan");
        Renderer *renderer = new Renderer(window);

        while (!window->shouldClose()) {
            glfwPollEvents();
            renderer->render();
        }

        delete renderer;

        delete window;

        glfwTerminate();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}