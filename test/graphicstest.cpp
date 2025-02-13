#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Gui.hpp"
#include "Renderer.hpp"
#include "Synthmania.hpp"

class Test : public Game {
   public:
    Test(int width, int height, const char* title) : Game() {
        this->audio = NULL;
        this->window = std::make_unique<Window>(width, height, title);
        window->setWindowUserPointer(this);
        this->renderer = std::make_unique<Renderer>(*this, this->getWindow());
        Device& device = renderer->getDevice();
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
        ComputeShader shader(device, readFile("bin/square.comp.spv"), "main",
                             128);
        CommandPool pool(device);
        ComputeModule module(pool, shader, {range}, {in, out},
                             {128 * sizeof(float), 128 * sizeof(float)});
        float arr[128];
        for (int i = 0; i < 128; i++) arr[i] = i * i;
        module.fillBuffer(0, arr);
        unsigned int constants[] = {126, 1};
        Queue q = *device.getQueue("compute");
        module.run(q, constants, 2 * sizeof(unsigned int), 128);
        module.emptyBuffer(1, arr);
        for (int i = 0; i < 128; i++) {
            std::cout << i * i << " squared = " << arr[i] << std::endl;
        }
    }

    void init() {
        Game::init();
        auto g = std::make_shared<Gui>(Texture(), "back");
        g->setSize({1.f, 1.f});
        addGui(g);
        int i = 0;
        for (auto t : this->getTextHandler().createText("Hell o", "Stupid", 55,
                                                        glm::vec2({-2, 0}))) {
            std::shared_ptr<Gui> gui =
                std::make_shared<Gui>(t.character.texture, "hi");
            gui->setPosition(t.pos);
            gui->setSize(t.size);
            addGui(gui);
        }

        for (Text t : renderer->getTextHandler().createText(
                 "Hello", "Stupid", 55, glm::vec2({-1, 0}))) {
            std::string name = "hi";
            name.append(std::to_string(i++));
            std::shared_ptr<Gui> gui =
                std::make_shared<Gui>(t.character.texture, name.c_str());
            gui->setColor(glm::vec4({1, 0, 0, 1}));
            gui->setPosition(t.pos);
            gui->setSize(t.size);
            addGui(gui);
        }
    }

    virtual void update() override {}

    virtual ~Test() {}

   private:
};

int main() {
    Test game(1920, 1080, "GraphicsTest");
    game.init();
    game.run();
    return 0;
}