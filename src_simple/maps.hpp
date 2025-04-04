
#ifndef SUDOFUN_MAPPINGS_HEADER
#define SUDOFUN_MAPPINGS_HEADER

#include <array>
#include <stdint.h>
#include <vector>

extern const std::array<std::vector<uint32_t>, 9> INIT_ROW_GROUPS;
extern const std::array<std::vector<uint32_t>, 9> INIT_COL_GROUPS;
extern const std::array<std::vector<uint32_t>, 9> INIT_BLK_GROUPS;
extern const std::vector<uint32_t> INIT_UNSOLVED_INDICES;

/**
 * @brief Convert flat indices into row/col/blk indices
 *
 *  (row, col, blk) of ith cell -> FLAT_TO_ROW_COL_BLK[i]
 *
 */
extern const std::array<std::array<uint32_t, 3>, 81> FLAT_TO_ROW_COL_BLK;

/**
 * @brief Convert row/col/blk indices to flat puzzle indices
 *
 *  ith row contains the ROW_TO_FLAT[i] puzzle indices
 *
 */
extern const std::array<std::array<uint32_t, 9>, 9> ROW_TO_FLAT;
extern const std::array<std::array<uint32_t, 9>, 9> COL_TO_FLAT;
extern const std::array<std::array<uint32_t, 9>, 9> BLK_TO_FLAT;

/**
 * @brief Which row/col/blk is included in a given (col/blk)/(row/blk)/(row/col)
 *
 * Notably:
 *      ROW_TO_COL = COL_TO_ROW
 *      ROW_TO_BLK = BLK_TO_ROW
 */
extern const std::array<std::array<uint32_t, 9>, 9> ROW_TO_COL;
extern const std::array<std::array<uint32_t, 3>, 9> ROW_TO_BLK;
extern const std::array<std::array<uint32_t, 9>, 9> COL_TO_ROW;
extern const std::array<std::array<uint32_t, 3>, 9> COL_TO_BLK;
extern const std::array<std::array<uint32_t, 3>, 9> BLK_TO_ROW;
extern const std::array<std::array<uint32_t, 3>, 9> BLK_TO_COL;

#endif