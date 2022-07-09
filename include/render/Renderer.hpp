#pragma once

class Renderer;

//#define NDEBUG

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>

#include "Game.hpp"
#include "stb_image.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <ft2build.h>

#define FT
#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <limits>
#include <map>
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
#include "Font.hpp"
#include "Framebuffer.hpp"
#include "Gui.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
#include "Instance.hpp"
#include "Judgement.hpp"
#include "Memory.hpp"
#include "Model.hpp"
#include "Note.hpp"
#include "ParentedGui.hpp"
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

// Font texture sizes
const unsigned long FONT_SIZE = 128;

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
    Renderer(Game* theGame, Window* window);

    void render();
    void loadTextures(std::map<std::string, std::string> textures,
                      std::map<std::string, std::vector<ulong>> fonts);

    std::vector<ImageView*> getTextures();
    std::vector<Font> getFonts();
    Character getCharacter(std::string fontName, ulong code);

    std::vector<Text> createText(std::string text, std::string fontName,
                                 double size, glm::vec2 start);

    void setStartTime(double start);

    VkPhysicalDevice* getPhysicalDevice();
    Device* getDevice();

    ~Renderer();

   private:
    Game* game;

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

    std::vector<Model*> models;
    std::vector<ImageView*> textures;
    std::vector<ShaderDescriptorSet*> textureAccessors;

    std::vector<Font> fonts;

    std::chrono::_V2::system_clock::time_point begTime =
        std::chrono::high_resolution_clock::now();
    double startTime = 0;

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

    bool hasStencilComponent(VkFormat format);

    Image* createTextureImage(const char* path);
    Image* createSamplerImage(int width, int height);

    ImageView* readTexture(const char* path, const char* name);

   public:
    void addTexture(Image* texture, const char* name);

    void loadFonts(std::map<std::string, std::vector<ulong>> fonts);

    Image* loadCharacter(FT_Face face, ulong character);

   private:
    void updateDescriptorSet(ShaderDescriptorSet* descriptor,
                             ImageView* texture, TextureSampler* sampler,
                             Buffer* uniformBuffer);

   public:
    void transitionImageLayout(Image* image, VkImageLayout oldLayout,
                               VkImageLayout newLayout);

    void convertImage(Image* src, VkImageLayout srcImageLayout, Image* dst,
                      VkImageLayout dstImageLayout, VkFilter filter);

   private:
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