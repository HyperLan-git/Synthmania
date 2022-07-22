#include "GraphicalEffectHandler.hpp"

extern "C" class DefaultEmptyAnimLib : public GraphicalEffectHandler {
   public:
    DefaultEmptyAnimLib(Synthmania* game) : GraphicalEffectHandler(game) {}

    void onSpawn(Gui* g) override {}

    void update(int64_t time) override {}

    size_t updateUBO(void*& ubo, int64_t time) override {
        UniformBufferObject* u = (UniformBufferObject*)ubo;
        uint64_t MPQ = game->getPartition().MPQ;
        double progress = ((time % MPQ) / (double)MPQ);
        double a = -100 * progress * (progress - 0.2);
        if (a < 0) a = 0;
        double zoom = 1 + .02 * a;
        u->view[0][0] = u->view[1][1] = zoom;
        return sizeof(UniformBufferObject);
    }

    ~DefaultEmptyAnimLib() = default;
};

extern "C" GraphicalEffectHandler* getEffectHandler(Synthmania* game) {
    return new DefaultEmptyAnimLib(game);
}
