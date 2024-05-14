#include "ChartHandler.hpp"

extern "C" class DefaultEmptyAnimLib : public ChartHandler {
   public:
    DefaultEmptyAnimLib(Gamemode* game) : ChartHandler(game) {}

    void onSpawn(std::shared_ptr<Gui>& g) override {}

    void update(int64_t time) override {}

    const std::string getVertShaderCode() override { return std::string(); }

    const std::string getGeomShaderCode() override { return std::string(); }

    const std::string getFragShaderCode() override { return std::string(); }

    const std::string getFinalVertShaderCode() override {
        return std::string();
    }

    const std::string getFinalGeomShaderCode() override {
        return std::string();
    }

    const std::string getFinalFragShaderCode() override {
        return std::string();
    }

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

extern "C" ChartHandler* getChartHandler(Gamemode* game) {
    return new DefaultEmptyAnimLib(game);
}
