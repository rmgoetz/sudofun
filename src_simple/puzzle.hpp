
#ifndef SUDOFUN_PUZZLE_CLASS_HEADER
#define SUDOFUN_PUZZLE_CLASS_HEADER

#include "clue.hpp"
#include "maps.hpp"
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
    // Companion data objects to track the unsolved row, column, and block members for
    // each puzzle index; values are in flat index
    std::array<std::vector<uint32_t>, 9> row_groups;
    std::array<std::vector<uint32_t>, 9> col_groups;
    std::array<std::vector<uint32_t>, 9> blk_groups;

    // Mappings between different kinds of indices
    static const std::array<std::array<uint32_t, 3>, 81> *flat_to_row_col_block;
    static const std::array<std::array<uint32_t, 9>, 9> *row_to_flat;
    static const std::array<std::array<uint32_t, 9>, 9> *col_to_flat;
    static const std::array<std::array<uint32_t, 9>, 9> *blk_to_flat;
    static const std::array<std::array<uint32_t, 9>, 9> *row_to_col;
    static const std::array<std::array<uint32_t, 9>, 9> *col_to_row;
    static const std::array<std::array<uint32_t, 3>, 9> *row_to_blk;
    static const std::array<std::array<uint32_t, 3>, 9> *col_to_blk;
    static const std::array<std::array<uint32_t, 3>, 9> *blk_to_row;
    static const std::array<std::array<uint32_t, 3>, 9> *blk_to_col;

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

    const uint32_t &getRowIndex(const uint32_t &flat_index);
    const uint32_t &getColIndex(const uint32_t &flat_index);
    const uint32_t &getBlkIndex(const uint32_t &flat_index);
    std::vector<uint32_t> *getRowGroup(const uint32_t &flat_idx);
    std::vector<uint32_t> *getColGroup(const uint32_t &flat_idx);
    std::vector<uint32_t> *getBlkGroup(const uint32_t &flat_idx);

    // Methods to remove indices or vectors of indices from the unsolved row/col/blk groups
    void removeFromGroups(const uint32_t &cut_index);
    void removeFromGroups(const std::vector<uint32_t> &cut_vector);

    // Strike functions
    void bitRemoveFromGroup(std::vector<uint32_t> *group, const uint32_t &strike_idx, const uint16_t &strike_val);
    void strikeFromPuzzle(const uint32_t &strike_idx);
    void strikeFromPuzzle(const std::vector<uint32_t> &strike_idx_vec);
    void strikeLatestFromPuzzle();

    // Bit operations
    uint16_t rowNeighborBits(const uint32_t &index);
    uint16_t colNeighborBits(const uint32_t &index);
    uint16_t blkNeighborBits(const uint32_t &index);
    std::tuple<uint32_t, uint32_t, std::vector<uint32_t>> leastPopularBit();

    // Updating the solved and unsolved indices tracker
    void resetSolvedIndices();

    //
    void checkUnsolved();

    void validPuzzle(bool *goodness);

    void printPuzzle();
    void printUnsolved();
    void printGroup(const std::array<std::vector<uint32_t>, 9> &group);
    void printRowMap();
    void printColMap();
    void printBlkMap();
};

#endif