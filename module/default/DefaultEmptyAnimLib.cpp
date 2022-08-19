#include "ChartHandler.hpp"

extern "C" class DefaultEmptyAnimLib : public ChartHandler {
   public:
    DefaultEmptyAnimLib(Synthmania* game) : GraphicalEffectHandler(game) {}

    void onSpawn(Gui* g) override {}

    void update(int64_t time) override {}

    std::string getVertShaderCode() override { return std::string(); }

    std::string getGeomShaderCode() override { return std::string(); }

    std::string getFragShaderCode() override { return std::string(); }

    std::string getFinalVertShaderCode() override { return std::string(); }

    std::string getFinalGeomShaderCode() override { return std::string(); }

    std::string getFinalFragShaderCode() override { return std::string(); }

    VkDeviceSize updateUBO(void*& ubo, int64_t time) override {
        return sizeof(UniformBufferObject);
    }

    VkDeviceSize updateFinalUBO(void*& ubo, int64_t time) override {
        return sizeof(UniformBufferObject);
    }

    VkDeviceSize getUBOSize() override { return sizeof(UniformBufferObject); }

    VkDeviceSize getFinalUBOSize() override {
        return sizeof(UniformBufferObject);
    }

    void freeUBO(void*& ubo) override {}

    void freeFinalUBO(void*& ubo) override {}

    virtual ~DefaultEmptyAnimLib() = default;
};

extern "C" ChartHandler* getChartHandler(Synthmania* game) {
    return new DefaultEmptyAnimLib(game);
}
