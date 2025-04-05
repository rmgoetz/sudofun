
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

    // Companion data objects to track the unsolved row, column, and block members for
    // each puzzle index; values are in flat index
    std::array<std::vector<uint32_t>, 9> row_u_groups;
    std::array<std::vector<uint32_t>, 9> col_u_groups;
    std::array<std::vector<uint32_t>, 9> blk_u_groups;

public:
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

    // Helpful functions related to indexing and index translation
    const uint32_t &refRowIndex(const uint32_t &flat_index);
    const uint32_t &refColIndex(const uint32_t &flat_index);
    const uint32_t &refBlkIndex(const uint32_t &flat_index);
    const std::array<uint32_t, 9> &refIndicesInRow(const uint32_t &row_index);
    const std::array<uint32_t, 9> &refIndicesInCol(const uint32_t &col_index);
    const std::array<uint32_t, 9> &refIndicesInBlk(const uint32_t &blk_index);
    std::vector<uint32_t> *ptrRowUGroup(const uint32_t &flat_idx);
    std::vector<uint32_t> *ptrColUGroup(const uint32_t &flat_idx);
    std::vector<uint32_t> *ptrBlkUGroup(const uint32_t &flat_idx);
    std::array<uint32_t, 9> *ptrRowInCol(const uint32_t &col_index);
    std::array<uint32_t, 9> *ptrColInRow(const uint32_t &row_index);
    std::array<uint32_t, 3> *ptrRowInBlk(const uint32_t &blk_index);
    std::array<uint32_t, 3> *ptrColInBlk(const uint32_t &blk_index);
    std::array<uint32_t, 3> *ptrBlkInRow(const uint32_t &row_index);
    std::array<uint32_t, 3> *ptrBlkInCol(const uint32_t &col_index);

    // Accessing slices of values
    const std::array<uint16_t, 9> &getValuesInRow(const uint32_t &row_index);
    const std::array<uint16_t, 9> &getValuesInCol(const uint32_t &col_index);
    const std::array<uint16_t, 9> &getValuesInBlk(const uint32_t &blk_index);
    std::array<uint16_t *, 9> ptrValuesInRow(const uint32_t &row_index);
    std::array<uint16_t *, 9> ptrValuesInCol(const uint32_t &col_index);
    std::array<uint16_t *, 9> ptrValuesInBlk(const uint32_t &blk_index);

    // Methods to remove indices or vectors of indices from the unsolved row/col/blk groups
    void removeFromUGroups(const uint32_t &cut_index);
    void removeFromUGroups(const std::vector<uint32_t> &cut_vector);

    // Strike functions
    void bitRemoveFromUGroup(std::vector<uint32_t> *group, const uint32_t &strike_idx, const uint16_t &strike_val);
    void strikeFromPuzzle(const uint32_t &strike_idx);
    void strikeFromPuzzle(const std::vector<uint32_t> &strike_idx_vec);
    void strikeLatestFromPuzzle();

    // Squeeze functions
    std::array<uint16_t, 3> uniqueBitsInSections(const uint32_t &row_index, bool is_row);
    std::array<uint16_t *, 6> blkValuesNotInRow(const uint32_t &blk_index, const uint32_t &row_index);
    std::array<uint16_t *, 6> blkValuesNotInCol(const uint32_t &blk_index, const uint32_t &col_index);

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
    void printUGroup(const std::array<std::vector<uint32_t>, 9> &group);
    void printRowMap();
    void printColMap();
    void printBlkMap();
};

#endif