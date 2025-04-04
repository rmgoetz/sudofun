
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
    bool loaded_clue;

public:
    // Companion data objects to track the unsolved row, column, and block neighbors for
    // each puzzle index
    std::array<std::vector<uint8_t>, 81> row_groups;
    std::array<std::vector<uint8_t>, 81> col_groups;
    std::array<std::vector<uint8_t>, 81> blk_groups;

    std::array<std::vector<uint8_t>, 9> row_groups_n;
    std::array<std::vector<uint8_t>, 9> col_groups_n;
    std::array<std::vector<uint8_t>, 9> blk_groups_n;


    // Companion objects to track which indices have been solved, and which haven't
    std::vector<uint8_t> latest_solved_indices;
    std::vector<uint8_t> unsolved_indices;

    // Constructor
    Puzzle();

    // Setting and getting the puzzle data
    void setValue(uint8_t index, uint16_t val);
    uint16_t getValue(uint8_t index);
    uint8_t numUnsolved();

    // Updating the row/col/blk groups
    void removeFromGroup(std::vector<uint8_t> *group_vec, const uint8_t &cut_index);
    void removeSolvedFromGroups();

    //
    void strikeFromGroup(
        std::array<std::vector<uint8_t>, 81> *group,
        std::vector<uint8_t> *group_vec,
        const uint8_t &cut_index,
        const uint16_t &ref_val);

    // Updating the solved and unsolved indices tracker
    void resetSolvedIndices();

    //
    void checkUnsolved();

    // Update from default puzzle with
    void addClueString(Clue *clue);

    std::tuple<uint8_t, uint8_t, std::vector<uint8_t>> leastPopularBit();

    void validPuzzle(bool *goodness);

    void printPuzzle();
    void printUnsolved();
};

#endif