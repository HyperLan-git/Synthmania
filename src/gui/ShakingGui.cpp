#include "ShakingGui.hpp"

ShakingGui::ShakingGui(ImageView* texture, const char* name, double amplitude)
    : Gui(texture, name) {
    this->amplitude = amplitude;
}

bool ShakingGui::update(int64_t time) {
    double r = std::rand() * (long double)amplitude / RAND_MAX - amplitude / 2.,
           r2 =
               std::rand() * (long double)amplitude / RAND_MAX - amplitude / 2.;
    setGraphicalPosition({r, r2});
    return Gui::update(time);
}