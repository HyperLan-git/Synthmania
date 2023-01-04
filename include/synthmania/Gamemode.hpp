#pragma once

class Gamemode;

#include <memory>

#include "Gui.hpp"

class Gamemode {
   public:
    Gamemode();

    // returns true if should end
    virtual bool update() = 0;

    // Gets called when a new screen element appears
    virtual void onSpawn(std::shared_ptr<Gui> &g);
    // Gets called whenever the clock gets readjusted
    virtual void onClockAdjust(int64_t value);
    // Gets called when user leaves the options menu
    virtual void onConfigChange();

    // TODO make ubo its own standalone object that will free itself
    // Graphical functions
    // These functions get called every frame
    // (modify only if you messed with shaders)
    virtual size_t updateUBO(void *&ubo);
    virtual void freeUBO(void *&ubo);

    virtual size_t updateFinalUBO(void *&ubo);
    virtual void freeFinalUBO(void *&ubo);

    virtual ~Gamemode();

   private:
};