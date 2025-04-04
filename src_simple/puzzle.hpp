
#ifndef PUZZLE_CLASS_HEADER
#define PUZZLE_CLASS_HEADER

#include "clue.hpp"
#include <array>
#include <vector>
#include <tuple>
#include <stdint.h>
#include <unordered_map>

class Puzzle
{
private:
    // Data attribute tracking the solution space for each square
    std::array<uint16_t, 81> data;
    bool loaded_clue;

public:
    // Companion data objects to track the unsolved row, column, and block neighbors for
    // each puzzle index; values are in flat index
    std::array<std::vector<uint32_t>, 9> row_groups;
    std::array<std::vector<uint32_t>, 9> col_groups;
    std::array<std::vector<uint32_t>, 9> blk_groups;

    // Mappings from the flat index to group indices (row/col/blk)
    std::unordered_map<uint32_t, uint32_t> row_map;
    std::unordered_map<uint32_t, uint32_t> col_map;
    std::unordered_map<uint32_t, uint32_t> blk_map;

    // Companion objects to track which indices have been solved, and which haven't
    std::vector<uint32_t> latest_solved_indices;
    std::vector<uint32_t> unsolved_indices;

    // Constructor
    Puzzle();

    // Update from default puzzle with
    void addClueString(Clue *clue);

    // Setting and getting the puzzle data
    void setValue(uint32_t index, uint16_t val);
    uint16_t getValue(uint32_t index);
    uint32_t numUnsolved();

    // Methods to remove indices or vectors of indices from the unsolved row/col/blk groups
    void removeFromGroups(const uint32_t &cut_value);
    void removeFromGroups(const std::vector<uint32_t> &cut_vector);

    // Strike functions
    void bitRemoveFromGroup(std::vector<uint32_t> *group, const uint32_t &strike_idx, const uint16_t &strike_val);
    void strikeFromPuzzle(const uint32_t &strike_idx);
    void strikeFromPuzzle(const std::vector<uint32_t> &strike_idx_vec);
    void strikeLatestFromPuzzle();

    // Updating the solved and unsolved indices tracker
    void resetSolvedIndices();

    //
    void checkUnsolved();

    std::tuple<uint32_t, uint32_t, std::vector<uint32_t>> leastPopularBit();

    void validPuzzle(bool *goodness);

    void printPuzzle();
    void printUnsolved();
    void printGroup(const std::array<std::vector<uint32_t>, 9> &group);
    void printRowMap();
    void printColMap();
    void printBlkMap();
};

#endif