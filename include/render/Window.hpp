#pragma once

class Window;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Instance.hpp"

class Window {
   public:
    Window(const uint32_t width, const uint32_t height, const char *title);

    bool hasResized();
    void onResize();
    void setResized(bool resized);

    bool shouldClose();

    GLFWwindow *getWindow();

    void getFramebufferSize(uint32_t *width, uint32_t *height);

    VkResult createSurface(Instance *instance,
                           const VkAllocationCallbacks *allocator,
                           VkSurfaceKHR *surface);

    void setWindowUserPointer(void *pointer);
    void setKeycallback(GLFWkeyfun fun);

    ~Window();

   private:
    GLFWwindow *window;
    bool resized = false;
};
