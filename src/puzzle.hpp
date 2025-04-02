
#ifndef PUZZLE_CLASS_HEADER
#define PUZZLE_CLASS_HEADER

#include "clue.hpp"
#include <array>
#include <vector>
#include <tuple>
#include <stdint.h>

class Puzzle
{
private:
    // Data attribute tracking the solution space for each square
    std::array<uint16_t, 81> data;

    // Companion data objects to track row, column, and block groups (in the flat index).
    std::array<std::vector<uint8_t>, 81> row_groups;
    std::array<std::vector<uint8_t>, 81> col_groups;
    std::array<std::vector<uint8_t>, 81> blk_groups;

    // Companion objects to track which indices have been solved, and which haven't
    std::vector<uint8_t> solved_indices;
    std::vector<uint8_t> unsolved_indices;

public:
    Puzzle();

    void setValue(uint8_t index, uint16_t val);
    uint16_t getValue(uint8_t index);

    void addClueString(Clue *clue);

    std::tuple<uint8_t, uint8_t, std::vector<uint8_t>> leastPopularBit();

    void validPuzzle(bool *goodness);
};

#endif