#include "Window.hpp"

// This is the only time I will do that
// I prefer having all header related stuff in headers
#include "Game.hpp"

static void framebufferResizeCallback(GLFWwindow *window, int width,
                                      int height) {
    Game *game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
    game->getWindow()->onResize();
}

Window::Window(const uint32_t width, const uint32_t height, const char *title) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
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

VkResult Window::createSurface(Instance *instance,
                               const VkAllocationCallbacks *allocator,
                               VkSurfaceKHR *surface) {
    return glfwCreateWindowSurface(*(instance->getInstance()), this->window,
                                   allocator, surface);
}

void Window::setResized(bool resized) { this->resized = resized; }

void Window::setWindowUserPointer(void *pointer) {
    glfwSetWindowUserPointer(window, pointer);
}

void Window::setKeycallback(GLFWkeyfun fun) { glfwSetKeyCallback(window, fun); }

void Window::setMousecallback(GLFWmousebuttonfun fun) {
    glfwSetMouseButtonCallback(window, fun);
}

void Window::setMouseWheelcallback(GLFWscrollfun fun) {
    glfwSetScrollCallback(window, fun);
}

glm::vec2 Window::getCursorPos() {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return glm::vec2(x, y);
}

bool Window::mousePressed(int button) {
    return glfwGetMouseButton(window, button);
}