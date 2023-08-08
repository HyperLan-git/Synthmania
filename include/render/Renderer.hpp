#pragma once

class Renderer;

#include "ComputeModule.hpp"
#include "Game.hpp"
#include "RenderModule.hpp"
#include "Shader.hpp"
#include "Swapchain.hpp"
#include "Texture.hpp"
#include "stb_image.h"

// Font texture sizes
const unsigned long FONT_SIZE = 128;

// Swapchain size
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

// XXX I kinda want to get rid of that vulkantutorial ass looking error handling
#ifdef DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

// XXX put that in the instance class?
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator);

class Renderer : public boost::noncopyable {
   public:
    Renderer(Game* theGame, Window* window);

    void render();

    void loadTextures(std::map<std::string, std::string> textures);

    void setStartTime(double start);

    VkPhysicalDevice* getPhysicalDevice();
    Device& getDevice();

    Instance& getInstance();

    TextHandler* getTextHandler();

    Model& addModel(Model&& m);

    void loadGuiShaders(std::string vShader, std::string gShader,
                        std::string fShader, VkDeviceSize guiUBOSize);

    void loadFinalShaders(std::string vShader, std::string gShader,
                          std::string fShader, VkDeviceSize guiUBOSize);

    glm::vec2 getVirtPos(glm::vec2 realPos);

    VkExtent2D getFramebufferSize();

    ~Renderer();

   private:
    Game* game = NULL;

    Window* window = NULL;

    std::unique_ptr<Instance> instance;
    VkSurfaceKHR surface = NULL;

    VkPhysicalDevice physicalDevice = NULL;
    std::unique_ptr<Device> device;

    Swapchain* swapchain = NULL;

    // TODO simplify this shit I should not have to create all of this just to
    // do a second pass
    // What if I want to do 5 passes?
    TexPtr renderImageView;
    TexPtr depthImageView;
    Framebuffer* framebuffer = NULL;
    CommandBuffer* renderCommandBuffer = NULL;
    Pipeline* renderPipeline = NULL;
    PipelineLayout* renderPipelineLayout = NULL;
    ShaderDescriptorSetLayout* renderLayout = NULL;
    ShaderDescriptorSet* renderDescriptor = NULL;
    ShaderDescriptorPool* renderDescriptorPool = NULL;
    Buffer* uniformBuffer = NULL;
    TextureSampler* sampler = NULL;
    RenderPass* renderPass = NULL;

    RenderModule* guiModule = NULL;
    RenderModule* objModule = NULL;

    Semaphore* imageAvailableSemaphore = NULL;

    CommandPool* commandPool = NULL;

    TextureSampler* textureSampler = NULL;

    Buffer* vertexBuffer = NULL;
    Buffer* indexBuffer = NULL;

    std::vector<Buffer*> uniformBuffers;
    std::vector<Buffer*> guiUniformBuffers;
    std::vector<Buffer*> constantBuffers;
    std::vector<Buffer*> guiConstantBuffers;

    ShaderDescriptorPool* pool = NULL;

    std::vector<CommandBuffer> commandBuffers;

    std::vector<Semaphore*> imageAvailableSemaphores;
    std::vector<Semaphore*> renderFinishedSemaphores;
    std::vector<Fence*> inFlightFences;
    uint32_t currentFrame = 0;

    std::vector<Model> models;
    std::unordered_map<Texture, TexPtr, Texture::HashFunction> textures;

    TextHandler* textHandler = NULL;

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

    std::shared_ptr<Image> createTextureImage(const char* path);

    TexPtr readTexture(const char* path, const char* name);

    void addTexture(std::shared_ptr<Image> texture, const char* name);

    void loadFonts(std::map<std::string, std::vector<unsigned long>> fonts);

    void updateDescriptorSet(ShaderDescriptorSet& descriptor,
                             ImageView& texture, TextureSampler& sampler,
                             Buffer& uniformBuffer);

    void transitionImageLayout(Image& image, VkImageLayout oldLayout,
                               VkImageLayout newLayout);

    void convertImage(Image& src, VkImageLayout srcImageLayout, Image& dst,
                      VkImageLayout dstImageLayout, VkFilter filter);

    void resizeFramebuffer();
    void copyBufferToImage(Buffer& buffer, Image& image, uint32_t width,
                           uint32_t height);
    void copyImage(Image& src, VkImageLayout srcLayout, Image& dst,
                   VkImageLayout dstLayout);
    void createVertexBuffer(VkDeviceSize size);
    void createIndexBuffer(VkDeviceSize size);
    void createUniformBuffers();
    void createDescriptorSets();

    void createCommandBuffers();

    void recordCommandBuffer(CommandBuffer& commandBuffer,
                             RenderPass& renderPass, Framebuffer& framebuffer);
    void drawScreenCommandBuffer(CommandBuffer& commandBuffer,
                                 RenderPass& renderPass,
                                 Framebuffer& framebuffer);
    void updateUniformBuffer(uint32_t currentImage);
    void drawFrame();
    void drawEntity(std::shared_ptr<Entity>& entity,
                    CommandBuffer& commandBuffer);
    void drawGui(std::shared_ptr<Gui>& gui, CommandBuffer& commandBuffer);
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