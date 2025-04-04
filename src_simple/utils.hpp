#ifndef SUDOKU_UTILS_HEADER
#define SUDOKU_UTILS_HEADER

#include <stdint.h>

namespace utils
{

    uint16_t countBits(uint16_t n);

    uint16_t valueToNineBit(uint8_t val);
}

#endif