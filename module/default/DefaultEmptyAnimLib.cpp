#include "GraphicalEffectHandler.hpp"

extern "C" class DefaultEmptyAnimLib : public GraphicalEffectHandler {
   public:
    DefaultEmptyAnimLib(Synthmania* game) : GraphicalEffectHandler(game) {}

    const char* getVertShaderCode() override { return NULL; }

    const char* getFragShaderCode() override { return NULL; }

    void onSpawn(Gui* g) override {}

    void update(int64_t time) override {}

    size_t updateUBO(void*& ubo, int64_t time) override {
        return sizeof(UniformBufferObject);
    }

    ~DefaultEmptyAnimLib() = default;
};

extern "C" GraphicalEffectHandler* getEffectHandler(Synthmania* game) {
    return new DefaultEmptyAnimLib(game);
}
