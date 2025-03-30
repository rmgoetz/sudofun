

#ifndef PUZZLE_CLASS_HEADER
#define PUZZLE_CLASS_HEADER

#include "clue.hpp"
#include <array>
#include <stdint.h>

class Puzzle
{
private:
    std::array<uint16_t, 81> data;

public:
    Puzzle() : data({511}) {} // bin(511) = 0000000111111111

    void setValue(uint8_t index, uint16_t val);
    uint16_t getValue(uint8_t index);
    void addClueString(Clue clue);
};

#endif