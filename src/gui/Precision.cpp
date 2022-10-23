#include "Precision.hpp"

Precision::Precision(ImageView* texture, std::string name, int64_t time,
                     int64_t delta, int64_t hit_window)
    : Gui(texture, name) {
    hit_time = time;
    this->delta = delta;
}

bool Precision::update(int64_t time) {
    uint64_t d = this->hit_time + LINGER_PRECISION - time;
    color.a = d / (double)LINGER_PRECISION;
    this->position.x = delta / (long double)HIT_WINDOW / 1.2;

    return this->hit_time + LINGER_PRECISION < time;
}