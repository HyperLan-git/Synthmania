#include "Window.hpp"

static void framebufferResizeCallback(GLFWwindow *window, int width,
                                      int height) {
    Window *win = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    win->onResize();
}

Window::Window(const uint32_t width, const uint32_t height, const char *title) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

bool Window::shouldClose() { return glfwWindowShouldClose(window); }

void Window::onResize() { resized = true; }

bool Window::hasResized() { return resized; }

Window::~Window() { glfwDestroyWindow(window); }

void Window::getFramebufferSize(uint32_t *width, uint32_t *height) {
    int x, y;
    glfwGetFramebufferSize(window, &x, &y);
    *width = x;
    *height = y;
}

VkResult Window::createSurface(VkInstance instance,
                               const VkAllocationCallbacks *allocator,
                               VkSurfaceKHR *surface) {
    return glfwCreateWindowSurface(instance, this->window, allocator, surface);
}

void Window::setResized(bool resized) { this->resized = resized; }
