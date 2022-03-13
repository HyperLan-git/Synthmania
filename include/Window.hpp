#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window
{
public:
    Window(const uint32_t width, const uint32_t height, const char *title);

    bool hasResized();
    void onResize();
    void setResized(bool resized);

    bool shouldClose();

    void cleanup();

    void getFramebufferSize(uint32_t *width, uint32_t *height);

    VkResult createSurface(VkInstance instance,
                           const VkAllocationCallbacks *allocator,
                           VkSurfaceKHR *surface);

private:
    GLFWwindow *window;
    bool resized = false;
};
