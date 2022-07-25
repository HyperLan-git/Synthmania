#include "GraphicalEffectHandler.hpp"

extern "C" class DefaultEmptyAnimLib : public GraphicalEffectHandler {
   public:
    DefaultEmptyAnimLib(Synthmania* game) : GraphicalEffectHandler(game) {}

    std::string getVertShaderCode() override { return std::string(); }

    std::string getFragShaderCode() override { return std::string(); }

    void onSpawn(Gui* g) override {}

    void update(int64_t time) override {}

    VkDeviceSize updateUBO(void*& ubo, int64_t time) override {
        return sizeof(UniformBufferObject);
    }

    VkDeviceSize getUBOSize() override { return sizeof(UniformBufferObject); }

    void freeUBO(void*& ubo) override {}

    ~DefaultEmptyAnimLib() = default;
};

extern "C" GraphicalEffectHandler* getEffectHandler(Synthmania* game) {
    return new DefaultEmptyAnimLib(game);
}
