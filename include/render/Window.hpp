#pragma once

class Window;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Instance.hpp"

class Window {
   public:
    Window(const uint32_t width, const uint32_t height, const char *title,
           bool fullscreen = false);

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    bool hasResized();
    void onResize();
    void setResized(bool resized);

    bool shouldClose();

    GLFWwindow *getWindow();

    // This function is FUCKED UP BRAH
    void getFramebufferSize(uint32_t *width, uint32_t *height);

    VkResult createSurface(Instance *instance,
                           const VkAllocationCallbacks *allocator,
                           VkSurfaceKHR *surface);

    void setWindowUserPointer(void *pointer);

    void setKeycallback(GLFWkeyfun fun);
    void setMousecallback(GLFWmousebuttonfun fun);
    void setMouseWheelcallback(GLFWscrollfun fun);
    void setTextcallback(GLFWcharfun fun);

    std::string getClipboard();
    void setClipboard(const std::string &data);

    void resetCallbacks();

    glm::vec2 getCursorPos();

    bool mousePressed(int button = GLFW_MOUSE_BUTTON_LEFT);

    void fullscreen(bool fullscreen = true);

    ~Window();

   private:
    GLFWwindow *window;
    bool resized = false;
};
