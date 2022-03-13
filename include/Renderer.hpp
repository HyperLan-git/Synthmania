#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include "Model.hpp"
#include "Window.hpp"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<Vertex> vertices = {
    { { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
    { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
    { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

static std::vector<char> readFile(const std::string &filename);
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);

static void framebufferResizeCallback(GLFWwindow *window, int width,
                                      int height);

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator);

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Renderer
{
public:
    void init();
    void cleanup();
    void initVulkan();
    void initWindow();
    void mainLoop();
    void createVertexBuffer();

private:
    Window *window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkSurfaceKHR surface;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    void createSyncObjects();
    void createInstance();
    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void setupDebugMessenger();
    void drawFrame();

    void cleanupSwapChain();
    void recreateSwapChain();

    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer,
                             uint32_t imageIndex);
    VkShaderModule createShaderModule(const std::vector<char> &code);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    std::vector<const char *> getRequiredExtensions();
    uint32_t findMemoryType(uint32_t typeFilter,
                            VkMemoryPropertyFlags properties);
    bool checkValidationLayerSupport();
};
