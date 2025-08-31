
#ifndef SUDOFUN_PUZZLE_CLASS_HEADER
#define SUDOFUN_PUZZLE_CLASS_HEADER

#include "clue.hpp"
#include "maps.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <tuple>
#include <numeric>

class Puzzle
{
private:
    // Data attribute tracking the solution space for each square
    std::array<uint16_t, 81> data;
    bool loaded_clue;

    // Companion data objects to track the unsolved row, column, and block members for
    // each puzzle index; values are in flat index
    std::array<std::vector<uint32_t>, 9> row_u_groups;
    std::array<std::vector<uint32_t>, 9> col_u_groups;
    std::array<std::vector<uint32_t>, 9> blk_u_groups;

public:
    // Companion objects to track which indices have been solved, and which haven't
    std::vector<uint32_t> latest_solved_indices;
    std::vector<uint32_t> unsolved_indices;

    std::array<uint32_t, 81> unsolved_index_map;

    // Constructor
    Puzzle()
    {
        // Initialize the puzzle data to be entirely unsolved
        data.fill(511); // bin(511) = 0000000111111111

        // Signify that we haven't loaded any clue yet
        loaded_clue = false;

        // Initialize helper members to hard-coded pre-computed vectors
        unsolved_indices = maps::INIT_UNSOLVED_INDICES;
        unsolved_index_map = maps::INIT_UNSOLVED_INDEX_MAP;
        row_u_groups = maps::INIT_ROW_GROUPS;
        col_u_groups = maps::INIT_COL_GROUPS;
        blk_u_groups = maps::INIT_BLK_GROUPS;

        // Latest solved is empty, but reserve enough space to never need re-allocation
        latest_solved_indices.reserve(81);
    }

    //--------------------------------------------------------------------------------------------//
    //--- Update from default puzzle with a clue string ------------------------------------------//
    //--------------------------------------------------------------------------------------------//

    /**
     * @brief Adds a clue to a blank initialized puzzle.
     *
     * @param clue A clue which defines the puzzle
     */
    void addClueString(StringClue *clue)
    {

        if (this->loaded_clue)
        {
            throw std::runtime_error("A clue has already been added to this puzzle");
        }

        size_t string_len = clue->clue_string.length();
        if ((string_len % 3 != 0) || (string_len == 0))
        {
            throw std::runtime_error("Clue has invalid length after validation?!");
        }

        // Iterate over triplet substrings of the cluestring
        for (uint32_t i = 0; i < string_len; i += 3)
        {
            std::string s = clue->clue_string.substr(i, 3);

            // Convert clues to their index/value combination, remembering that cluestrings will
            // index starting at 1, not zero
            uint32_t row_idx = static_cast<uint32_t>(s[0] - '0') - 1;
            uint32_t col_idx = static_cast<uint32_t>(s[1] - '0') - 1;
            uint32_t val_num = static_cast<uint32_t>(s[2] - '0');
            uint32_t flat_index = row_idx * 9 + col_idx;

            // Set the corresponding flat index to the value converted to a 9-bit representation
            this->setValue(flat_index, utils::valueToNineBit(val_num));

            // Add to the list of solved
            this->latest_solved_indices.push_back(flat_index);

            // Remove from the unsolved list
            for (auto it = this->unsolved_indices.begin(); it != this->unsolved_indices.end(); ++it)
            {
                if (*it == flat_index)
                {
                    this->unsolved_indices.erase(it);
                    break;
                }
            }
        }

        this->removeVecFromUGroups(this->latest_solved_indices);
    }

    void addClueVector(WindowClue *clue)
    {

        if (this->loaded_clue)
        {
            throw std::runtime_error("A clue has already been added to this puzzle");
        }

        for (auto i = clue->clue_vector.begin(); i != clue->clue_vector.end(); ++i)
        {
            auto [flat_index, val_num] = *i;

            // Set the corresponding flat index to the value converted to a 9-bit representation
            this->setValue(flat_index, utils::valueToNineBit(val_num));

            // Add to the list of solved
            this->latest_solved_indices.push_back(flat_index);

            // Remove from the unsolved list
            for (auto it = this->unsolved_indices.begin(); it != this->unsolved_indices.end(); ++it)
            {
                if (*it == flat_index)
                {
                    this->unsolved_indices.erase(it);
                    break;
                }
            }
        }

        this->removeVecFromUGroups(this->latest_solved_indices);
    }

    /**
     * @brief The conventional form for puzzle strings is an 81 character long string where . represents
     * a null entry.
     *
     * @param benchmarkString
     */
    void addBenchmarkString(const std::string &benchmarkString)
    {
        uint32_t removedCount{0};
        auto itBegin = this->unsolved_indices.begin();
        for (uint32_t flat_index = 0; flat_index < 81; ++flat_index)
        {
            char s = benchmarkString[flat_index];
            if (s != '.')
            {
                // Set the corresponding flat index to the value converted to a 9-bit representation
                this->setValue(flat_index, utils::valueToNineBit(static_cast<uint32_t>(s - '0')));

                // Add to the list of solved
                this->latest_solved_indices.push_back(flat_index);

                // Remove from the unsolved list
                this->unsolved_indices.erase(itBegin + flat_index - removedCount);

                ++removedCount;
            }
        }

        this->removeVecFromUGroups(this->latest_solved_indices);
    }

    //--------------------------------------------------------------------------------------------//
    //--- Setting and getting the puzzle data ----------------------------------------------------//
    //--------------------------------------------------------------------------------------------//

    void setValue(uint32_t flat_index, uint16_t val)
    {
        data[flat_index] = val;
    }

    uint16_t getValue(uint32_t flat_index)
    {
        return data[flat_index];
    }

    uint16_t *ptrValue(uint32_t flat_index)
    {
        return &data[flat_index];
    }

    /**
     * @brief Returns the number of still unsolved elements of the puzzle.
     *
     * @return uint32_t
     */
    uint32_t numUnsolved()
    {
        return this->unsolved_indices.size();
    }

    /**
     * @brief Looks over the unsolved indices to see whether any are actually solved. If one is solved,
     * remove it from the unsolved indices and add it to the recently solved indices.
     *
     */
    void checkUnsolved()
    {
        // Check which flat indices have been solved and put them into the latest solved vector
        uint32_t index_in_unsolved = 0;
        std::vector<uint32_t> indices_to_remove;
        indices_to_remove.reserve(this->unsolved_indices.size());
        for (const uint32_t &puzzle_index : this->unsolved_indices)
        {
            if (utils::zeroIfOneOrNoBits(this->getValue(puzzle_index)) == 0)
            {
                // Add this index to latest solved
                this->latest_solved_indices.push_back(puzzle_index);

                // Push this index into the tracker
                indices_to_remove.push_back(index_in_unsolved);
            }

            ++index_in_unsolved;
        }

        // Iterate in reverse through the indices to remove from the unsolved indices
        for (auto it = indices_to_remove.rbegin(); it != indices_to_remove.rend(); ++it)
        {
            this->unsolved_indices.erase(this->unsolved_indices.begin() + *it);
        }
    }

    /**
     * @brief Determines whether a puzzle is still valid by checking whether any entry has value 0.
     *
     * @param goodness
     */
    bool validPuzzle()
    {
        bool goodness{true};

        for (const uint16_t &val : this->data)
        {
            goodness &= (val != 0);
        }

        return goodness;
    }

    //--------------------------------------------------------------------------------------------//
    //--- Helpful functions related to indexing and index translation ----------------------------//
    //--------------------------------------------------------------------------------------------//

    /**
     * @brief Returns the flat indices of all members of a given row.
     *
     * @param row_index
     * @return const std::array<uint32_t, 9>&
     */
    const std::array<uint32_t, 9> &refIndicesInRow(uint32_t row_index)
    {
        return maps::ROW_TO_FLAT[row_index];
    }

    /**
     * @brief Returns the flat indices of all members of a given column.
     *
     * @param col_index
     * @return const std::array<uint32_t, 9>&
     */
    const std::array<uint32_t, 9> &refIndicesInCol(uint32_t col_index)
    {
        return maps::COL_TO_FLAT[col_index];
    }

    /**
     * @brief Returns the flat indices of the unsolved elements in the same row as a given
     * flat index.
     *
     * @param flat_idx
     * @return std::vector<uint32_t>*
     */
    std::vector<uint32_t> *ptrRowUGroup(uint32_t flat_idx)
    {
        return &this->row_u_groups[maps::flatToRow(flat_idx)];
    }

    /**
     * @brief Returns the flat indices of the unsolved elements in the same column as a given
     * flat index.
     *
     * @param flat_idx
     * @return std::vector<uint32_t>*
     */
    std::vector<uint32_t> *ptrColUGroup(uint32_t flat_idx)
    {
        return &this->col_u_groups[maps::flatToCol(flat_idx)];
    }

    /**
     * @brief Returns the flat indices of the unsolved elements in the same block as a given
     * flat index.
     *
     * @param flat_idx
     * @return std::vector<uint32_t>*
     */
    std::vector<uint32_t> *ptrBlkUGroup(uint32_t flat_idx)
    {
        return &this->blk_u_groups[maps::flatToBlk(flat_idx)];
    }

    //--------------------------------------------------------------------------------------------//
    //--- Accessing slices of values -------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------//

    /**
     * @brief Returns pointers to the values of each element of a given row.
     *
     * @param row_index
     * @return std::array<uint16_t *, 9>
     */
    std::array<uint16_t *, 9> ptrValuesInRow(uint32_t row_index)
    {
        std::array<uint16_t *, 9> vals_in_row;

        for (uint32_t n = 0; n < 9; ++n)
        {
            vals_in_row[n] = this->ptrValue(this->refIndicesInRow(row_index)[n]);
        }

        return vals_in_row;
    }

    /**
     * @brief Returns pointers to the values of each element of a given column.
     *
     * @param col_index
     * @return std::array<uint16_t *, 9>
     */
    std::array<uint16_t *, 9> ptrValuesInCol(uint32_t col_index)
    {
        std::array<uint16_t *, 9> vals_in_col;

        for (uint32_t n = 0; n < 9; ++n)
        {
            vals_in_col[n] = this->ptrValue(this->refIndicesInCol(col_index)[n]);
        }

        return vals_in_col;
    }

    //--------------------------------------------------------------------------------------------//
    //--- Methods to remove indices or vectors of indices from the unsolved row/col/blk groups ---//
    //--------------------------------------------------------------------------------------------//

    /**
     * @brief
     *
     * @param group
     * @param cut_value
     */
    static void removeFromGroup(std::vector<uint32_t> *group, uint32_t cut_index)
    {
        // If the cut index is in the group, remove it (there will be at most one instance).
        // NOTE: using std::find instead of this loop fails to compile on my build system
        for (auto it = group->begin(); it != group->end(); ++it)
        {
            if (*it == cut_index)
            {
                group->erase(it);
                break;
            }
        }
    }

    void removeFromURowGroupUnsafe(uint32_t row_index, uint32_t cut_index)
    {
        uint32_t erase_idx{0};
        uint32_t idx{0};
        for (auto it = row_u_groups[row_index].begin(); it != row_u_groups[row_index].end(); ++it, ++idx)
        {
            erase_idx += idx * ((*it - cut_index) == 0);
        }
        row_u_groups[row_index].erase(row_u_groups[row_index].begin() + erase_idx);
    }

    void removeFromUColGroupUnsafe(uint32_t col_index, uint32_t cut_index)
    {
        uint32_t erase_idx{0};
        uint32_t idx{0};
        for (auto it = col_u_groups[col_index].begin(); it != col_u_groups[col_index].end(); ++it, ++idx)
        {
            erase_idx += idx * ((*it - cut_index) == 0);
        }
        col_u_groups[col_index].erase(col_u_groups[col_index].begin() + erase_idx);
    }

    void removeFromUBlkGroupUnsafe(uint32_t blk_index, uint32_t cut_index)
    {
        uint32_t erase_idx{0};
        uint32_t idx{0};
        for (auto it = blk_u_groups[blk_index].begin(); it != blk_u_groups[blk_index].end(); ++it, ++idx)
        {
            erase_idx += idx * ((*it - cut_index) == 0);
        }
        blk_u_groups[blk_index].erase(blk_u_groups[blk_index].begin() + erase_idx);
    }

    /**
     * @brief Removes a group of flat indices from the unsolved groups.
     *
     * @param cut_vector
     */
    void removeVecFromUGroups(const std::vector<uint32_t> &cut_vector)
    {
        for (const uint32_t &cut_index : cut_vector)
        {
            // Convert to row, col, and blk indices
            uint32_t row_idx = maps::flatToRow(cut_index);
            uint32_t col_idx = maps::flatToCol(cut_index);
            uint32_t blk_idx = maps::flatToBlk(cut_index);

            // If the cut index is in the group, remove it (there will be at most one instance).
            // NOTE: using std::find instead of this loop fails to compile on my build system
            removeFromURowGroupUnsafe(row_idx, cut_index);
            removeFromUColGroupUnsafe(col_idx, cut_index);
            removeFromUBlkGroupUnsafe(blk_idx, cut_index);
            // removeFromGroup(&this->row_u_groups[row_idx], cut_index);
            // removeFromGroup(&this->col_u_groups[col_idx], cut_index);
            // removeFromGroup(&this->blk_u_groups[blk_idx], cut_index);
        }
    }

    //--------------------------------------------------------------------------------------------//
    //--- Strike functions -----------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------//

    /**
     * @brief Strikes a value from all members of the group except at the strike index.
     *
     * @param group One of the unsolved row, column, or block groups for a specific index
     * @param strike_idx The index of the value being struck from the group
     * @param strike_val The value to remove from the group
     */
    void bitRemoveFromUGroup(std::vector<uint32_t> *group, uint32_t strike_idx, uint16_t strike_val)
    {
        uint32_t remove_offset = 9;
        uint32_t count = 0;

        // Remove the bit from every other member of the group
        for (auto it = group->begin(); it != group->end(); ++it)
        {
            if (strike_idx != *it)
            {
                this->data[*it] -= this->data[*it] & strike_val;
                ++count;
            }
            else
            {
                remove_offset = count;
            }
        }

        // Remove the corresponding struck cell from the group
        if (remove_offset < 9) // TODO: this check should be unnecessary
        {
            group->erase(group->begin() + remove_offset);
        }
    }

    /**
     * @brief For the most recently solved puzzle elements, remove their bit from all of their groups.
     *
     */
    void strikeLatestFromPuzzle()
    {
        for (const uint32_t &strike_idx : this->latest_solved_indices)
        {
            uint32_t row_idx = maps::flatToRow(strike_idx);
            uint32_t col_idx = maps::flatToCol(strike_idx);
            uint32_t blk_idx = maps::flatToBlk(strike_idx);
            uint16_t puzzle_value = this->getValue(strike_idx);

            this->bitRemoveFromUGroup(&this->row_u_groups[row_idx], strike_idx, puzzle_value);
            this->bitRemoveFromUGroup(&this->col_u_groups[col_idx], strike_idx, puzzle_value);
            this->bitRemoveFromUGroup(&this->blk_u_groups[blk_idx], strike_idx, puzzle_value);
        }
    }

    //--------------------------------------------------------------------------------------------//
    //--- Squeeze functions ----------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------//

    /**
     * @brief Divide every row or column into three sections according to block membership, and identify bits
     * which are unique to any particular section of the row/column.
     *
     * @param row_or_col_index
     * @param is_row
     * @return std::array<uint16_t, 3>
     */
    std::array<uint16_t, 3> uniqueBitsBySections(uint32_t row_or_col_index, bool is_row)
    {
        // Puzzle values in the row or column
        std::array<uint16_t *, 9> vals;

        if (is_row)
        {
            vals = this->ptrValuesInRow(row_or_col_index);
        }
        else
        {
            vals = this->ptrValuesInCol(row_or_col_index);
        }

        // Determine what bits are present in each section
        uint16_t section_1_bits = 0;
        uint16_t section_2_bits = 0;
        uint16_t section_3_bits = 0;
        for (uint32_t i = 0; i < 3; ++i)
        {
            section_1_bits |= *vals[i];
            section_2_bits |= *vals[i + 3];
            section_3_bits |= *vals[i + 6];
        }

        // Identify any bits unique to a section
        uint16_t section_1_unq = ((section_2_bits | section_3_bits) ^ 511) & section_1_bits;
        uint16_t section_2_unq = ((section_1_bits | section_3_bits) ^ 511) & section_2_bits;
        uint16_t section_3_unq = ((section_1_bits | section_2_bits) ^ 511) & section_3_bits;

        return {section_1_unq, section_2_unq, section_3_unq};
    }

    /**
     * @brief Returns the unsolved puzzle values in a given block that are not in the specified row.
     *
     * @param blk_index
     * @param row_index
     * @return std::vector<uint16_t *>
     */
    std::vector<uint16_t *> uBlkValuesNotInRow(uint32_t blk_index, uint32_t row_index)
    {
        std::vector<uint16_t *> not_in_row;

        // WE ASSUME THE ROW INTERSECTS THE BLOCK
        not_in_row.reserve(6);

        // Loop over unsolved elements of the block
        for (const uint32_t &flat_idx : this->blk_u_groups[blk_index])
        {
            // The row index of this element
            uint32_t i = maps::flatToRow(flat_idx);

            // If this row isn't the one we're trying to avoid, add it to the list
            if (i != row_index)
            {
                not_in_row.push_back(this->ptrValue(flat_idx));
            }
        }

        return not_in_row;
    }

    /**
     * @brief Returns the unsolved puzzle values in a given block that are not in the specified column.
     *
     * @param blk_index
     * @param col_index
     * @return std::vector<uint16_t *>
     */
    std::vector<uint16_t *> uBlkValuesNotInCol(uint32_t blk_index, uint32_t col_index)
    {
        std::vector<uint16_t *> not_in_col;

        // WE ASSUME THE COL INTERSECTS THE BLOCK
        not_in_col.reserve(6);

        // Loop over unsolved elements of the block
        for (const uint32_t &flat_idx : this->blk_u_groups[blk_index])
        {
            // The col index of this element
            uint32_t i = maps::flatToCol(flat_idx);

            // If this col isn't the one we're trying to avoid, add the puzzle value
            if (i != col_index)
            {
                not_in_col.push_back(this->ptrValue(flat_idx));
            }
        }

        return not_in_col;
    }

    //--------------------------------------------------------------------------------------------//
    //--- Pipe functions -------------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------//

    /**
     * @brief Sort all of the unsolved elements of a given block into two groups: those included in a given row, and
     * those not.
     *
     * @param blk_index
     * @param row_index
     * @return std::tuple<std::vector<uint16_t *>, std::vector<uint16_t *>>
     */
    std::tuple<std::vector<uint16_t *>, std::vector<uint16_t *>> uBlkValuesSiftRow(uint32_t blk_index, uint32_t row_index)
    {
        std::vector<uint16_t *> in_row;
        std::vector<uint16_t *> not_in_row;

        // WE ASSUME THE ROWS CORRESPOND TO ROWS THAT THE BLOCK INTERSECTS WITH
        in_row.reserve(3);
        not_in_row.reserve(6);

        // Loop over unsolved elements of the block
        for (const uint32_t &flat_idx : this->blk_u_groups[blk_index])
        {
            // The col index of this element
            uint32_t i = maps::flatToRow(flat_idx);

            // Separate elements based on row inclusion
            if (i == row_index)
            {
                in_row.push_back(this->ptrValue(flat_idx));
            }
            else
            {
                not_in_row.push_back(this->ptrValue(flat_idx));
            }
        }

        return {in_row, not_in_row};
    }

    /**
     * @brief Sort all of the unsolved elements of a given block into two groups: those included in a given column, and
     * those not.
     *
     * @param blk_index
     * @param col_index
     * @return std::tuple<std::vector<uint16_t *>, std::vector<uint16_t *>>
     */
    std::tuple<std::vector<uint16_t *>, std::vector<uint16_t *>> uBlkValuesSiftCol(uint32_t blk_index, uint32_t col_index)
    {
        std::vector<uint16_t *> in_col;
        std::vector<uint16_t *> not_in_col;

        // WE ASSUME THE COLS CORRESPOND TO COLS THAT THE BLOCK INTERSECTS WITH
        in_col.reserve(3);
        not_in_col.reserve(6);

        // Loop over unsolved elements of the block
        for (const uint32_t &flat_idx : this->blk_u_groups[blk_index])
        {
            // The col index of this element
            uint32_t i = maps::flatToCol(flat_idx);

            // Separate elements based on col inclusion
            if (i == col_index)
            {
                in_col.push_back(this->ptrValue(flat_idx));
            }
            else
            {
                not_in_col.push_back(this->ptrValue(flat_idx));
            }
        }

        return {in_col, not_in_col};
    }

    /**
     * @brief Return all of the values for unsolved elements in a given row which are not in a given block.
     *
     * @param row_index
     * @param blk_index
     * @return std::vector<uint16_t *>
     */
    std::vector<uint16_t *> uRowValuesNotInBlk(uint32_t row_index, uint32_t blk_index)
    {
        std::vector<uint16_t *> not_in_blk;

        // WE ASSUME THE ROW INTERSECTS WITH THE BLOCK
        not_in_blk.reserve(6);

        // Loop over unsolved elements of the row
        for (const uint32_t &flat_idx : this->row_u_groups[row_index])
        {
            // The blk index of this element
            uint32_t g = maps::flatToBlk(flat_idx);

            // Add if the element is not in the blk
            if (g != blk_index)
            {
                not_in_blk.push_back(this->ptrValue(flat_idx));
            }
        }

        return not_in_blk;
    }

    /**
     * @brief Return all of the values for unsolved elements in a given column which are not in a given block.
     *
     * @param col_index
     * @param blk_index
     * @return std::vector<uint16_t *>
     */
    std::vector<uint16_t *> uColValuesNotInBlk(uint32_t col_index, uint32_t blk_index)
    {
        std::vector<uint16_t *> not_in_blk;

        // WE ASSUME THE COLS CORRESPOND TO COLS THAT THE BLOCK INTERSECTS WITH
        not_in_blk.reserve(6);

        // Loop over unsolved elements of the col
        for (const uint32_t &flat_idx : this->col_u_groups[col_index])
        {
            // The blk index of this element
            uint32_t g = maps::flatToBlk(flat_idx);

            // Add if the element is not in the blk
            if (g != blk_index)
            {
                not_in_blk.push_back(this->ptrValue(flat_idx));
            }
        }

        return not_in_blk;
    }

    //--------------------------------------------------------------------------------------------//
    //--- Bit operations -------------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------//

    /**
     * @brief Find any bits at the flat index which are unique to its row, column, or block.
     *
     * @param flat_index
     * @return uint16_t
     */
    uint16_t uniqueUGroupBits(uint32_t flat_index)
    {
        uint16_t unique_bits_row = this->getValue(flat_index);
        uint16_t unique_bits_col = this->getValue(flat_index);
        uint16_t unique_bits_blk = this->getValue(flat_index);
        for (const uint32_t &row_neighbor_idx : *this->ptrRowUGroup(flat_index))
        {
            uint16_t same_idx = (flat_index - row_neighbor_idx) == 0;
            unique_bits_row &= (unique_bits_row * same_idx) | (unique_bits_row ^ this->getValue(row_neighbor_idx));
        }
        for (const uint32_t &col_neighbor_idx : *this->ptrColUGroup(flat_index))
        {
            uint16_t same_idx = (flat_index - col_neighbor_idx) == 0;
            unique_bits_col &= (unique_bits_col * same_idx) | (unique_bits_col ^ this->getValue(col_neighbor_idx));
        }
        for (const uint32_t &blk_neighbor_idx : *this->ptrBlkUGroup(flat_index))
        {
            uint16_t same_idx = (flat_index - blk_neighbor_idx) == 0;
            unique_bits_blk &= (unique_bits_blk * same_idx) | (unique_bits_blk ^ this->getValue(blk_neighbor_idx));
        }

        return unique_bits_row | unique_bits_col | unique_bits_blk;
    }

    /**
     * @brief For a given flat index, determine which bits it contains that are not included among
     * any other unsolved row members.
     *
     * @param flat_index
     * @return uint16_t
     */
    uint16_t uniqueURowBits(uint32_t flat_index)
    {
        uint16_t unique_bits = this->getValue(flat_index);
        for (const uint32_t &row_neighbor_idx : *this->ptrRowUGroup(flat_index))
        {
            uint16_t same_idx = (flat_index - row_neighbor_idx) == 0;
            unique_bits &= (unique_bits * same_idx) | (unique_bits ^ this->getValue(row_neighbor_idx));
        }

        return unique_bits;
    }

    /**
     * @brief For a given flat index, determine which bits it contains that are not included among
     * any other unsolved col members.
     *
     * @param flat_index
     * @return uint16_t
     */
    uint16_t uniqueUColBits(uint32_t flat_index)
    {
        uint16_t unique_bits = this->getValue(flat_index);
        for (const uint32_t &col_neighbor_idx : *this->ptrColUGroup(flat_index))
        {
            uint16_t same_idx = (flat_index - col_neighbor_idx) == 0;
            unique_bits &= (unique_bits * same_idx) | (unique_bits ^ this->getValue(col_neighbor_idx));
        }

        return unique_bits;
    }

    /**
     * @brief For a given flat index, determine which bits it contains that are not included among
     * any other unsolved blk members.
     *
     * @param flat_index
     * @return uint16_t
     */
    uint16_t uniqueUBlkBits(uint32_t flat_index)
    {
        uint16_t unique_bits = this->getValue(flat_index);
        for (const uint32_t &blk_neighbor_idx : *this->ptrBlkUGroup(flat_index))
        {
            uint16_t same_idx = (flat_index - blk_neighbor_idx) == 0;
            unique_bits &= (unique_bits * same_idx) | (unique_bits ^ this->getValue(blk_neighbor_idx));
        }

        return unique_bits;
    }

    /**
     * @brief Sort bits by their multiplicity in the unsolved list, and also return their locations
     * in the puzzle.
     *
     * @return std::tuple<std::vector<uint16_t>, std::vector<uint32_t>, std::vector<uint32_t>> The bit index, multiplicity, and locations in the puzzle
     */
    std::tuple<std::vector<uint16_t>, std::vector<std::vector<uint32_t>>> leastPopularBit()
    {
        std::array<uint16_t, 9> bits;
        std::array<uint32_t, 9> multiplicity = {};
        std::array<std::vector<uint32_t>, 9> locations;

        // Count bit multiplicity for each bit amongst the unsolved elements
        for (const uint32_t &unsolved_idx : this->unsolved_indices)
        {
            for (uint16_t i = 0; i < 9; ++i)
            {
                bits[i] = 1 << i;

                if ((this->getValue(unsolved_idx) & (1 << i)) != 0)
                {
                    multiplicity[i]++;
                    locations[i].push_back(unsolved_idx);
                }
            }
        }

        // Determine the index map which sort multiplicity by ascending value
        // (least popular bits first)
        std::vector<size_t> sort_map(9);
        std::iota(sort_map.begin(), sort_map.end(), 0);
        std::sort(sort_map.begin(), sort_map.end(), [&](size_t a, size_t b)
                  { return multiplicity[a] < multiplicity[b]; });

        // We want to remove anything with multiplicity of zero, and order the return
        // by ascending multiplicity
        std::vector<uint16_t> bits_v;
        std::vector<std::vector<uint32_t>> loc_v;
        bits_v.reserve(9);
        loc_v.reserve(9);
        for (auto i : sort_map)
        {
            if (multiplicity[i] > 0)
            {
                bits_v.push_back(bits[i]);
                loc_v.push_back(locations[i]);
            }
        }

        return {bits_v, loc_v};
    }

    //--------------------------------------------------------------------------------------------//
    //--- Updating the solved and unsolved indices tracker ---------------------------------------//
    //--------------------------------------------------------------------------------------------//

    void resetSolvedIndices()
    {
        this->latest_solved_indices.clear();
    }

    //--------------------------------------------------------------------------------------------//
    //---
    //--------------------------------------------------------------------------------------------//

    //--------------------------------------------------------------------------------------------//
    //--- Printing functions ---------------------------------------------------------------------//
    //--------------------------------------------------------------------------------------------//

    void printPuzzle(bool nine_bit = true)
    {
        for (uint32_t i = 0; i < 81; ++i)
        {
            if (nine_bit)
            {
                std::cout << std::setw(3) << this->getValue(i) << " ";
            }
            else
            {
                std::cout << std::setw(3) << utils::nineBitToValue(this->getValue(i)) << " ";
            }
            if ((i + 1) % 9 == 0)
            {
                std::cout << std::endl;

                if ((i + 1) % 27 == 0)
                {
                    std::cout << "\n"
                              << std::endl;
                    std::cout.flush();
                }
            }
            else if ((i + 1) % 3 == 0)
            {
                std::cout << " ";
                std::cout.flush();
            }
            else
            {
                std::cout.flush();
            }
        }
    }

    void printUnsolved()
    {
        for (const uint32_t &index : this->unsolved_indices)
        {
            std::cout << (int)index << " ";
        }

        std::cout << std::endl;
    }

    void printUGroup(const std::array<std::vector<uint32_t>, 9> &group)
    {
        for (uint32_t i = 0; i < 9; ++i)
        {
            for (const uint32_t &elem : group[i])
            {
                std::cout << std::setw(2) << (int)elem << " ";
                std::cout.flush();
            }

            std::cout << std::endl;
        }
    }

    void printRowMap()
    {
        for (uint32_t k = 0; k < 81; ++k)
        {
            std::cout << std::setw(2) << (int)k << " -> " << (int)maps::flatToRow(k) << "     ";
            if ((k + 1) % 9 == 0)
            {
                std::cout << std::endl;
            }
            else
            {
                std::cout.flush();
            }
        }
    }

    void printColMap()
    {
        for (uint32_t k = 0; k < 81; ++k)
        {
            std::cout << std::setw(2) << (int)k << " -> " << (int)maps::flatToCol(k) << "     ";
            if ((k + 1) % 9 == 0)
            {
                std::cout << std::endl;
            }
            else
            {
                std::cout.flush();
            }
        }
    }

    void printBlkMap()
    {
        for (uint32_t k = 0; k < 81; ++k)
        {
            std::cout << std::setw(2) << (int)k << " -> " << (int)maps::flatToBlk(k) << "     ";
            if ((k + 1) % 9 == 0)
            {
                std::cout << std::endl;
            }
            else
            {
                std::cout.flush();
            }
        }
    }
};

#endif