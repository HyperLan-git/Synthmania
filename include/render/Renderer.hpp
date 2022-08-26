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
#include <map>

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
#include "ChartHandler.hpp"
#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "ComputeModule.hpp"
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
                      std::map<std::string, std::vector<unsigned long>> fonts);

    std::vector<ImageView*> getTextures();
    std::vector<Font> getFonts();
    Character getCharacter(std::string fontName, unsigned long code);

    std::vector<Text> createText(std::string text, std::string fontName,
                                 double size, glm::vec2 start);
    std::vector<Text> createVerticalText(std::string text, std::string fontName,
                                         double size, glm::vec2 start);

    void setStartTime(double start);

    VkPhysicalDevice* getPhysicalDevice();
    Device* getDevice();

    Instance* getInstance();

    void addModel(Model* m);

    void loadGuiShaders(std::string vShader, std::string gShader,
                        std::string fShader, VkDeviceSize guiUBOSize);

    void loadFinalShaders(std::string vShader, std::string gShader,
                          std::string fShader, VkDeviceSize guiUBOSize);

    glm::vec2 getVirtPos(glm::vec2 realPos);

    ~Renderer();

   private:
    Game* game = NULL;

    Window* window = NULL;

    Instance* instance = NULL;
    VkSurfaceKHR* surface = NULL;

    VkPhysicalDevice physicalDevice = NULL;
    Device* device = NULL;

    Swapchain* swapchain = NULL;

    // TODO simplify this shit I should not have to create all of this just to
    // do a second pass
    // What if I want to do 5 passes?
    Image* renderImage = NULL;
    ImageView* renderImageView = NULL;
    Image* depthImage = NULL;
    ImageView* depthImageView = NULL;
    Framebuffer* framebuffer = NULL;
    CommandBuffer* renderCommandBuffer = NULL;
    Pipeline* renderPipeline = NULL;
    PipelineLayout* renderPipelineLayout = NULL;
    ShaderDescriptorSetLayout* renderLayout = NULL;
    ShaderDescriptorSet* renderDescriptor = NULL;
    ShaderDescriptorPool* renderDescriptorPool = NULL;
    Semaphore* imageAvailableSemaphore = NULL;
    Buffer* uniformBuffer = NULL;
    TextureSampler* sampler = NULL;

    ShaderDescriptorSetLayout* shaderLayout = NULL;
    ShaderDescriptorSetLayout* guiShaderLayout = NULL;

    std::vector<ShaderDescriptorSet*> descriptorSets;
    std::vector<ShaderDescriptorSet*> guiDescriptorSets;

    CommandPool* commandPool = NULL;

    TextureSampler* textureSampler = NULL;
    TextureSampler* guiSampler = NULL;

    Buffer* vertexBuffer = NULL;
    Buffer* indexBuffer = NULL;

    PipelineLayout* graphicsPipelineLayout = NULL;
    Pipeline* graphicsPipeline = NULL;

    PipelineLayout* guiPipelineLayout = NULL;
    Pipeline* guiPipeline = NULL;

    std::vector<Buffer*> uniformBuffers;
    std::vector<Buffer*> guiUniformBuffers;
    std::vector<Buffer*> constantBuffers;
    std::vector<Buffer*> guiConstantBuffers;

    ShaderDescriptorPool* pool = NULL;
    ShaderDescriptorPool* guiPool = NULL;

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

    VkDeviceSize guiUBOSize = sizeof(UniformBufferObject);
    std::string guiVertShader = "bin/gui.vert.spv",
                guiGeomShader = "bin/def.geom.spv",
                guiFragShader = "bin/def.frag.spv";

    VkDeviceSize finalUBOSize = sizeof(UniformBufferObject);
    std::string finalVertShader = "bin/pass.vert.spv",
                finalGeomShader = "bin/pass.geom.spv",
                finalFragShader = "bin/pass.frag.spv";

    void initWindow();

    void initVulkan();

    void createSwapchain();
    void recreateSwapchain();

    void createInstance();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void createImageViews();

    void createRenderPass();

    void createDescriptorSetLayout();

    void createGraphicsPipeline();
    void createGuiPipeline();
    void createMainPipeline();

    bool hasStencilComponent(VkFormat format);

    Image* createTextureImage(const char* path);
    Image* createSamplerImage(int width, int height);

    ImageView* readTexture(const char* path, const char* name);

   public:
    void addTexture(Image* texture, const char* name);

    void loadFonts(std::map<std::string, std::vector<unsigned long>> fonts);

    Image* loadCharacter(FT_Face face, unsigned long character);

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

    void createCommandBuffers();

    void recordCommandBuffer(CommandBuffer* commandBuffer,
                             RenderPass* renderPass, Framebuffer* framebuffer,
                             VkExtent2D extent);
    void drawScreenCommandBuffer(CommandBuffer* commandBuffer,
                                 RenderPass* renderPass,
                                 Framebuffer* framebuffer, VkExtent2D extent);
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