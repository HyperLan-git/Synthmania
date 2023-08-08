#include "Window.hpp"

// This is the only time I will do that
// I prefer having all header related stuff in headers
#include "Game.hpp"

static void framebufferResizeCallback(GLFWwindow *window, int width,
                                      int height) {
    void *ptr = glfwGetWindowUserPointer(window);
    if (ptr == NULL) return;
    Game *game = reinterpret_cast<Game *>(ptr);
    game->getWindow()->onResize();
}

Window::Window(const uint32_t width, const uint32_t height, const char *title,
               bool fullscreen) {
    glfwInit();

    glfwSetErrorCallback([](int err, const char *desc) {
        std::cerr << "glfw err : " << desc << std::endl;
    });

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window =
        glfwCreateWindow(width, height, title,
                         fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    glfwSetWindowUserPointer(window, NULL);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

bool Window::shouldClose() { return glfwWindowShouldClose(window); }

void Window::onResize() { resized = true; }

bool Window::hasResized() { return resized; }

void Window::getFramebufferSize(uint32_t *width, uint32_t *height) {
    int x, y;
    glfwGetFramebufferSize(window, &x, &y);
    *width = x;
    *height = y;
}

VkResult Window::createSurface(Instance *instance,
                               const VkAllocationCallbacks *allocator,
                               VkSurfaceKHR *surface) {
    return glfwCreateWindowSurface(instance->getInstance(), this->window,
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

void Window::setTextcallback(GLFWcharfun fun) {
    glfwSetCharCallback(window, fun);
}

void Window::resetCallbacks() {
    setMouseWheelcallback(NULL);
    setMousecallback(NULL);
    setKeycallback(NULL);
    setTextcallback(NULL);
}

std::string Window::getClipboard() {
    return std::string(glfwGetClipboardString(window));
}

void Window::setClipboard(const std::string &data) {
    glfwSetClipboardString(window, data.c_str());
}

glm::vec2 Window::getCursorPos() {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return glm::vec2(x, y);
}

bool Window::mousePressed(int button) {
    return glfwGetMouseButton(window, button);
}

void Window::fullscreen(bool fullscreen) {
    if (!fullscreen) {
        uint32_t w, h;
        getFramebufferSize(&w, &h);
        glfwSetWindowMonitor(window, NULL, 0, 0, w, h, GLFW_DONT_CARE);
        return;
    }
    GLFWmonitor *m = glfwGetWindowMonitor(window);
    if (m == NULL) m = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(m);
    glfwSetWindowMonitor(window, m, 0, 0, mode->width, mode->height,
                         GLFW_DONT_CARE);
}

Window::~Window() {
    if (window) glfwDestroyWindow(window);
}