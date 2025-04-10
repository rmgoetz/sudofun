#ifndef SUDOFUN_UTILS_HEADER
#define SUDOFUN_UTILS_HEADER

#include <stdint.h>
#include <vector>

namespace utils
{
    uint16_t countBits(uint16_t n);

    uint16_t valueToNineBit(uint32_t val);

    uint16_t nineBitToValue(uint16_t nb);

    uint16_t bitsInSrcNotInRef(std::vector<uint16_t *> src, std::vector<uint16_t *> ref);
}

#endif