#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Gui.hpp"
#include "Renderer.hpp"

class Test : public Game {
   public:
    Test(int width, int height, const char* title) : Game() {
        this->window = new Window(width, height, title);
        this->renderer = new Renderer(this, window);
        Device* device = renderer->getDevice();
        VkPushConstantRange range{.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                                  .offset = 0,
                                  .size = sizeof(unsigned int) * 2};
        VkDescriptorSetLayoutBinding in, out;
        in.binding = 0;
        in.descriptorCount = 1;
        in.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        in.pImmutableSamplers = NULL;
        in.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        out.binding = 1;
        out.descriptorCount = 1;
        out.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        out.pImmutableSamplers = NULL;
        out.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        VkDescriptorSetLayoutBinding bindings[] = {in, out};
        VkDeviceSize bufSizes[] = {128 * sizeof(float), 128 * sizeof(float)};
        ComputeShader shader(device, readFile("bin/square.comp.spv"), "main",
                             128);
        CommandPool pool(*(renderer->getPhysicalDevice()), device);
        ComputeModule module(renderer->getPhysicalDevice(), device, &pool,
                             &shader, &range, 1, bindings, 2, bufSizes, 2);
        float arr[128];
        for (int i = 0; i < 128; i++) arr[i] = i;
        module.fillBuffer(0, arr);
        unsigned int constants[] = {126, 1};
        module.run(device->getQueue("compute"), constants,
                   2 * sizeof(unsigned int), 128);
        module.emptyBuffer(1, arr);
        for (int i = 0; i < 128; i++) {
            std::cout << i << " squared = " << arr[i] << std::endl;
        }
    }

    void init() {
        Gui* g = new Gui(getTextureByName(renderer->getTextures(), "missing"),
                         "back");
        g->setSize({5.f, 5.f});
        addGui(g);
        int i = 0;
        for (Text t : textHandler->createText("Hello", "Stupid", 55,
                                              glm::vec2({-1, 0}))) {
            std::string name = "hi";
            name.append(std::to_string(i++));
            Gui* gui = new Gui(t.character.texture, name.c_str());
            gui->setColor(glm::vec4({1, 0, 0, 1}));
            gui->setPosition(t.pos);
            gui->setSize(t.size);
            addGui(gui);
        }
    }

    void update() {}

    ~Test() {}

   private:
};

int main() {
    Test* game = new Test(1920, 1080, "GraphicsTest");
    game->run();
    delete game;
    return 0;
}