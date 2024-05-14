#pragma once

#include <cstdint>
#include <string>

struct Score {
    std::string chart, charter, diff;
    uint32_t score, perfects, misses, near, far;
};