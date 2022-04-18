#pragma once
//#define NDEBUG

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include "Buffer.hpp"
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Device.hpp"
#include "Entity.hpp"
#include "Fence.hpp"
#include "Framebuffer.hpp"
#include "Gui.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
#include "Instance.hpp"
#include "Memory.hpp"
#include "Model.hpp"
#include "Note.hpp"
#include "Pipeline.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"
#include "Semaphore.hpp"
#include "Shader.hpp"
#include "ShaderDescriptorPool.hpp"
#include "ShaderDescriptorSet.hpp"
#include "Swapchain.hpp"
#include "TextureSampler.hpp"
#include "Utils.hpp"
#include "Window.hpp"

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator);

class Renderer {
   public:
    Renderer(Window* window);

    void render();

    ~Renderer();

   private:
    Window* window;

    Instance* instance;
    VkSurfaceKHR* surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    Device* device;

    Swapchain* swapchain;

    ShaderDescriptorSetLayout* shaderLayout = nullptr;
    ShaderDescriptorSetLayout* guiShaderLayout = nullptr;

    std::vector<ShaderDescriptorSet*> descriptorSets;
    std::vector<ShaderDescriptorSet*> guiDescriptorSets;

    CommandPool* commandPool;

    TextureSampler* textureSampler;
    TextureSampler* guiSampler;

    Buffer* vertexBuffer;
    Buffer* indexBuffer;

    PipelineLayout* graphicsPipelineLayout;
    Pipeline* graphicsPipeline;

    PipelineLayout* guiPipelineLayout;
    Pipeline* guiPipeline;

    std::vector<Buffer*> uniformBuffers;
    std::vector<Buffer*> guiUniformBuffers;
    std::vector<Buffer*> constantUniformBuffers;
    std::vector<Buffer*> guiConstantUniformBuffers;

    ShaderDescriptorPool* pool;
    ShaderDescriptorPool* guiPool;

    std::vector<CommandBuffer*> commandBuffers;

    std::vector<Semaphore*> imageAvailableSemaphores;
    std::vector<Semaphore*> renderFinishedSemaphores;
    std::vector<Fence*> inFlightFences;
    uint32_t currentFrame = 0;

    std::vector<Entity*> entities;
    std::vector<Gui*> guis;
    std::vector<Model*> models;
    std::vector<ImageView*> textures;
    std::vector<ShaderDescriptorSet*> textureAccessors;

    Model* guiModel;

    void initWindow();

    void initVulkan();
    void recreateSwapchain();

    void createInstance();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void createImageViews();

    void createRenderPass();

    void createDescriptorSetLayout();

    void createGraphicsPipeline();
    void createGuiPipeline();

    void addGui(Gui* gui);

    bool hasStencilComponent(VkFormat format);

    Image* createTextureImage(const char* path);
    Image* createSamplerImage(int width, int height);

    ImageView* readTexture(const char* path, const char* name);

    void addTexture(Image* texture, const char* name);

    void updateDescriptorSet(ShaderDescriptorSet* descriptor,
                             ImageView* texture, TextureSampler* sampler,
                             Buffer* uniformBuffer);

    void transitionImageLayout(Image* image, VkImageLayout oldLayout,
                               VkImageLayout newLayout);
    void copyBufferToImage(Buffer* buffer, Image* image, uint32_t width,
                           uint32_t height);
    void copyImage(Image* src, VkImageLayout srcLayout, Image* dst,
                   VkImageLayout dstLayout);
    void createVertexBuffer(VkDeviceSize size);
    void createIndexBuffer(VkDeviceSize size);
    void createUniformBuffers();
    void createDescriptorSets();

    void recordCommandBuffer(CommandBuffer* commandBuffer, uint32_t imageIndex);
    void updateUniformBuffer(uint32_t currentImage);
    void drawFrame();
    void drawEntity(Entity* entity, CommandBuffer* commandBuffer);
    void drawGui(Gui* gui, CommandBuffer* commandBuffer);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
};