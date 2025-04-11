
#include "puzzle.hpp"
#include "clue.hpp"
#include "maps.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

// Static class members
const std::array<std::array<uint32_t, 3>, 81> *Puzzle::flat_to_row_col_block = &FLAT_TO_ROW_COL_BLK;
const std::array<std::array<uint32_t, 9>, 9> *Puzzle::row_to_flat = &ROW_TO_FLAT;
const std::array<std::array<uint32_t, 9>, 9> *Puzzle::col_to_flat = &COL_TO_FLAT;
const std::array<std::array<uint32_t, 9>, 9> *Puzzle::blk_to_flat = &BLK_TO_FLAT;
const std::array<std::array<uint32_t, 9>, 9> *Puzzle::row_to_col = &ROW_TO_COL;
const std::array<std::array<uint32_t, 9>, 9> *Puzzle::col_to_row = &COL_TO_ROW;
const std::array<std::array<uint32_t, 3>, 9> *Puzzle::row_to_blk = &ROW_TO_BLK;
const std::array<std::array<uint32_t, 3>, 9> *Puzzle::col_to_blk = &COL_TO_BLK;
const std::array<std::array<uint32_t, 3>, 9> *Puzzle::blk_to_row = &BLK_TO_ROW;
const std::array<std::array<uint32_t, 3>, 9> *Puzzle::blk_to_col = &BLK_TO_COL;

Puzzle::Puzzle()
{
    // Initialize the puzzle data to be entirely unsolved
    data.fill(511); // bin(511) = 0000000111111111

    // Signify that we haven't loaded any clue yet
    loaded_clue = false;

    // Initialize helper members to hard-coded pre-computed vectors
    unsolved_indices = INIT_UNSOLVED_INDICES;
    row_u_groups = INIT_ROW_GROUPS;
    col_u_groups = INIT_COL_GROUPS;
    blk_u_groups = INIT_BLK_GROUPS;
}

/**
 * @brief Adds a clue to a blank initialized puzzle.
 *
 * @param clue A clue which defines the puzzle
 */
void Puzzle::addClueString(Clue *clue)
{

    if (this->loaded_clue)
    {
        throw std::runtime_error("A clue has already been added to this puzzle");
    }

    for (auto i = clue->begin(); i != clue->end(); ++i)
    {
        // Grab the three-character-long string corresponding to a single cell clue
        std::string s = *i;

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

    this->removeFromUGroups(this->latest_solved_indices);
}

void Puzzle::setValue(uint32_t index, uint16_t val)
{
    data.at(index) = val;
}

uint16_t Puzzle::getValue(uint32_t index)
{
    return data.at(index);
}

uint16_t *Puzzle::ptrValue(const uint32_t &index)
{
    return &data.at(index);
}

/**
 * @brief Returns the number of still unsolved elements of the puzzle.
 *
 * @return uint32_t
 */
uint32_t Puzzle::numUnsolved()
{
    return this->unsolved_indices.size();
}

/**
 * @brief Returns the row index of the element located at a given flat index.
 *
 * @param flat_index
 * @return const uint32_t&
 */
const uint32_t &Puzzle::refRowIndex(const uint32_t &flat_index)
{
    return this->flat_to_row_col_block->at(flat_index).at(0);
}

/**
 * @brief Returns the column index of the element located at a given flat index.
 *
 * @param flat_index
 * @return const uint32_t&
 */
const uint32_t &Puzzle::refColIndex(const uint32_t &flat_index)
{
    return this->flat_to_row_col_block->at(flat_index).at(1);
}

/**
 * @brief Returns the block index of the element located at a given flat index.
 *
 * @param flat_index
 * @return const uint32_t&
 */
const uint32_t &Puzzle::refBlkIndex(const uint32_t &flat_index)
{
    return this->flat_to_row_col_block->at(flat_index).at(2);
}

/**
 * @brief Returns the flat indices of all members of a given row.
 *
 * @param row_index
 * @return const std::array<uint32_t, 9>&
 */
const std::array<uint32_t, 9> &Puzzle::refIndicesInRow(const uint32_t &row_index)
{
    return row_to_flat->at(row_index);
}

/**
 * @brief Returns the flat indices of all members of a given column.
 *
 * @param col_index
 * @return const std::array<uint32_t, 9>&
 */
const std::array<uint32_t, 9> &Puzzle::refIndicesInCol(const uint32_t &col_index)
{
    return col_to_flat->at(col_index);
}

/**
 * @brief Returns the flat indices of the unsolved elements in the same row as a given
 * flat index.
 *
 * @param flat_idx
 * @return std::vector<uint32_t>*
 */
std::vector<uint32_t> *Puzzle::ptrRowUGroup(const uint32_t &flat_idx)
{
    return &this->row_u_groups.at(this->refRowIndex(flat_idx));
}

/**
 * @brief Returns the flat indices of the unsolved elements in the same column as a given
 * flat index.
 *
 * @param flat_idx
 * @return std::vector<uint32_t>*
 */
std::vector<uint32_t> *Puzzle::ptrColUGroup(const uint32_t &flat_idx)
{
    return &this->col_u_groups.at(this->refColIndex(flat_idx));
}

/**
 * @brief Returns the flat indices of the unsolved elements in the same block as a given
 * flat index.
 *
 * @param flat_idx
 * @return std::vector<uint32_t>*
 */
std::vector<uint32_t> *Puzzle::ptrBlkUGroup(const uint32_t &flat_idx)
{
    return &this->blk_u_groups.at(this->refBlkIndex(flat_idx));
}

/**
 * @brief Returns pointers to the values of each element of a given row.
 *
 * @param row_index
 * @return std::array<uint16_t *, 9>
 */
std::array<uint16_t *, 9> Puzzle::ptrValuesInRow(const uint32_t &row_index)
{
    std::array<uint16_t *, 9> vals_in_row;

    for (uint32_t n = 0; n < 9; ++n)
    {
        vals_in_row[n] = this->ptrValue(this->refIndicesInRow(row_index).at(n));
    }

    return vals_in_row;
}

/**
 * @brief Returns pointers to the values of each element of a given column.
 *
 * @param col_index
 * @return std::array<uint16_t *, 9>
 */
std::array<uint16_t *, 9> Puzzle::ptrValuesInCol(const uint32_t &col_index)
{
    std::array<uint16_t *, 9> vals_in_col;

    for (uint32_t n = 0; n < 9; ++n)
    {
        vals_in_col[n] = this->ptrValue(this->refIndicesInCol(col_index).at(n));
    }

    return vals_in_col;
}

/**
 * @brief
 *
 * @param group
 * @param cut_value
 */
void removeFromGroup(std::vector<uint32_t> *group, const uint32_t &cut_index)
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

/**
 * @brief Removes a given flat index from the unsolved groups
 *
 * @param cut_index
 */
void Puzzle::removeFromUGroups(const uint32_t &cut_index)
{
    // Convert to row, col, and blk indices
    uint32_t row_idx = this->flat_to_row_col_block->at(cut_index).at(0);
    uint32_t col_idx = this->flat_to_row_col_block->at(cut_index).at(1);
    uint32_t blk_idx = this->flat_to_row_col_block->at(cut_index).at(2);

    // If the cut index is in the group, remove it (there will be at most one instance).
    // NOTE: using std::find instead of this loop fails to compile on my build system
    removeFromGroup(&this->row_u_groups.at(row_idx), cut_index);
    removeFromGroup(&this->col_u_groups.at(col_idx), cut_index);
    removeFromGroup(&this->blk_u_groups.at(blk_idx), cut_index);
}

/**
 * @brief Removes a group of flat indices from the unsolved groups.
 *
 * @param cut_vector
 */
void Puzzle::removeFromUGroups(const std::vector<uint32_t> &cut_vector)
{
    for (const uint32_t &cut_index : cut_vector)
    {
        this->removeFromUGroups(cut_index);
    }
}

/**
 * @brief Strikes a value from all members of the group except at the strike index.
 *
 * @param group One of the unsolved row, column, or block groups for a specific index
 * @param strike_idx The index of the value being struck from the group
 * @param strike_val The value to remove from the group
 */
void Puzzle::bitRemoveFromUGroup(std::vector<uint32_t> *group, const uint32_t &strike_idx, const uint16_t &strike_val)
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
    if (remove_offset < 9)
    {
        group->erase(group->begin() + remove_offset);
    }
}

/**
 * @brief For a solved cell specified by index, remove the bit from all of its groups.
 *
 * @param strike_idx Flat index in puzzle
 */
void Puzzle::strikeFromPuzzle(const uint32_t &strike_idx)
{
    uint32_t row_idx = this->refRowIndex(strike_idx);
    uint32_t col_idx = this->refColIndex(strike_idx);
    uint32_t blk_idx = this->refBlkIndex(strike_idx);
    uint16_t puzzle_value = this->getValue(strike_idx);

    this->bitRemoveFromUGroup(&this->row_u_groups.at(row_idx), strike_idx, puzzle_value);
    this->bitRemoveFromUGroup(&this->col_u_groups.at(col_idx), strike_idx, puzzle_value);
    this->bitRemoveFromUGroup(&this->blk_u_groups.at(blk_idx), strike_idx, puzzle_value);
}

/**
 * @brief For a set of solved cells specified by index, remove their bit from all of their groups.
 *
 * @param strike_idx_vec Flat indices in puzzle
 */
void Puzzle::strikeFromPuzzle(const std::vector<uint32_t> &strike_idx_vec)
{
    for (const uint32_t &strike_idx : strike_idx_vec)
    {
        this->strikeFromPuzzle(strike_idx);
    }
}

/**
 * @brief For the most recently solved puzzle elements, remove their bit from all of their groups.
 *
 */
void Puzzle::strikeLatestFromPuzzle()
{
    this->strikeFromPuzzle(this->latest_solved_indices);
}

/**
 * @brief Divide every row or column into three sections according to block membership, and identify bits
 * which are unique to any particular section of the row/column.
 *
 * @param row_or_col_index
 * @param is_row
 * @return std::array<uint16_t, 3>
 */
std::array<uint16_t, 3> Puzzle::uniqueBitsInSections(const uint32_t &row_or_col_index, bool is_row)
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
        section_1_bits |= *vals.at(i);
        section_2_bits |= *vals.at(i + 3);
        section_3_bits |= *vals.at(i + 6);
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
std::vector<uint16_t *> Puzzle::uBlkValuesNotInRow(const uint32_t &blk_index, const uint32_t &row_index)
{
    std::vector<uint16_t *> not_in_row;

    // Loop over unsolved elements of the block
    for (const uint32_t &flat_idx : this->blk_u_groups.at(blk_index))
    {
        // The row index of this element
        const uint32_t &i = this->refRowIndex(flat_idx);

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
std::vector<uint16_t *> Puzzle::uBlkValuesNotInCol(const uint32_t &blk_index, const uint32_t &col_index)
{
    std::vector<uint16_t *> not_in_col;

    // Loop over unsolved elements of the block
    for (const uint32_t &flat_idx : this->blk_u_groups.at(blk_index))
    {
        // The col index of this element
        const uint32_t &i = this->refColIndex(flat_idx);

        // If this col isn't the one we're trying to avoid, add the puzzle value
        if (i != col_index)
        {
            not_in_col.push_back(this->ptrValue(flat_idx));
        }
    }

    return not_in_col;
}

/**
 * @brief Sort all of the unsolved elements of a given block into two groups: those included in a given row, and
 * those not.
 *
 * @param blk_index
 * @param row_index
 * @return std::tuple<std::vector<uint16_t *>, std::vector<uint16_t *>>
 */
std::tuple<std::vector<uint16_t *>, std::vector<uint16_t *>> Puzzle::uBlkValuesSiftRow(const uint32_t &blk_index, const uint32_t &row_index)
{
    std::vector<uint16_t *> in_row;
    std::vector<uint16_t *> not_in_row;

    // Loop over unsolved elements of the block
    for (const uint32_t &flat_idx : this->blk_u_groups.at(blk_index))
    {
        // The col index of this element
        const uint32_t &i = this->refRowIndex(flat_idx);

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
std::tuple<std::vector<uint16_t *>, std::vector<uint16_t *>> Puzzle::uBlkValuesSiftCol(const uint32_t &blk_index, const uint32_t &col_index)
{
    std::vector<uint16_t *> in_col;
    std::vector<uint16_t *> not_in_col;

    // Loop over unsolved elements of the block
    for (const uint32_t &flat_idx : this->blk_u_groups.at(blk_index))
    {
        // The col index of this element
        const uint32_t &i = this->refColIndex(flat_idx);

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
std::vector<uint16_t *> Puzzle::uRowValuesNotInBlk(const uint32_t &row_index, const uint32_t &blk_index)
{
    std::vector<uint16_t *> not_in_blk;

    // Loop over unsolved elements of the row
    for (const uint32_t &flat_idx : this->row_u_groups.at(row_index))
    {
        // The blk index of this element
        const uint32_t &g = this->refBlkIndex(flat_idx);

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
std::vector<uint16_t *> Puzzle::uColValuesNotInBlk(const uint32_t &col_index, const uint32_t &blk_index)
{
    std::vector<uint16_t *> not_in_blk;

    // Loop over unsolved elements of the col
    for (const uint32_t &flat_idx : this->col_u_groups.at(col_index))
    {
        // The blk index of this element
        const uint32_t &g = this->refBlkIndex(flat_idx);

        // Add if the element is not in the blk
        if (g != blk_index)
        {
            not_in_blk.push_back(this->ptrValue(flat_idx));
        }
    }

    return not_in_blk;
}

/**
 * @brief Retrieves all bits in the unsolved row neighbors of an element
 * with a specified flat index.
 *
 * @param flat_index
 * @return uint16_t
 */
uint16_t Puzzle::rowNeighborBits(const uint32_t &flat_index)
{
    uint16_t neighbor_bits = 0;
    // Accumulate all of the bits found amongst row neighbors
    for (const uint32_t &neighbor_idx : *this->ptrRowUGroup(flat_index))
    {
        if (flat_index != neighbor_idx)
        {
            neighbor_bits |= this->getValue(neighbor_idx);
        }
    }

    return neighbor_bits;
}

/**
 * @brief Retrieves all bits in the unsolved column neighbors of an element
 * with a specified flat index.
 *
 * @param flat_index
 * @return uint16_t
 */
uint16_t Puzzle::colNeighborBits(const uint32_t &flat_index)
{
    uint16_t neighbor_bits = 0;
    // Accumulate all of the bits found amongst col neighbors
    for (const uint32_t &neighbor_idx : *this->ptrColUGroup(flat_index))
    {
        if (flat_index != neighbor_idx)
        {
            neighbor_bits |= this->getValue(neighbor_idx);
        }
    }

    return neighbor_bits;
}

/**
 * @brief Retrieves all bits in the unsolved block neighbors of an element
 * with a specified flat index.
 *
 * @param flat_index
 * @return uint16_t
 */
uint16_t Puzzle::blkNeighborBits(const uint32_t &flat_index)
{
    uint16_t neighbor_bits = 0;
    // Accumulate all of the bits found amongst blk neighbors
    for (const uint32_t &neighbor_idx : *this->ptrBlkUGroup(flat_index))
    {
        if (flat_index != neighbor_idx)
        {
            neighbor_bits |= this->getValue(neighbor_idx);
        }
    }

    return neighbor_bits;
}

void Puzzle::resetSolvedIndices()
{
    this->latest_solved_indices.clear();
}

/**
 * @brief Looks over the unsolved indices to see whether any are actually solved. If one is solved,
 * remove it from the unsolved indices and add it to the recently solved indices.
 *
 */
void Puzzle::checkUnsolved()
{
    // Check which flat indices have been solved and put them into the latest solved vector
    uint32_t index_in_unsolved = 0;
    std::vector<uint32_t> indices_to_remove;
    for (const uint32_t &puzzle_index : this->unsolved_indices)
    {
        if (utils::countBits(this->getValue(puzzle_index)) == 1)
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
 * @brief
 *
 * @return std::tuple<std::vector<uint16_t>, std::vector<uint32_t>, std::vector<uint32_t>> The bit index, multiplicity, and locations in the puzzle
 */
std::tuple<std::vector<uint16_t>, std::vector<std::vector<uint32_t>>> Puzzle::leastPopularBit()
{
    std::array<uint16_t, 9> bits;
    std::array<uint32_t, 9> multiplicity;
    std::array<std::vector<uint32_t>, 9> locations;

    // Count bit multiplicity for each bit amongst the unsolved elements
    for (const uint32_t &unsolved_idx : this->unsolved_indices)
    {
        for (uint16_t i = 0; i < 9; ++i)
        {
            bits[i] = 1 << i;

            if (this->getValue(unsolved_idx) & 1 << i != 0)
            {
                multiplicity[i]++;
                locations[i].push_back(unsolved_idx);
            }
        }
    }

    // Sort the bits and locations by multiplicity, and then multiplicity itself
    std::sort(bits.begin(), bits.end(), [&](uint16_t a, uint16_t b)
              { return multiplicity[a] < multiplicity[b]; });
    std::sort(locations.begin(), locations.end(), [&](std::vector<uint32_t> a, std::vector<uint32_t> b)
              { uint32_t a_index = std::find(locations.begin(), locations.end(), a) - locations.begin();
                uint32_t b_index = std::find(locations.begin(), locations.end(), b) - locations.begin();
                return multiplicity.at(a_index) < multiplicity.at(b_index); });
    std::sort(multiplicity.begin(), multiplicity.end(), std::greater<uint32_t>());

    // We want to remove anything with multiplicity of zero
    std::vector<uint16_t> bits_v;
    std::vector<std::vector<uint32_t>> loc_v;
    for (uint32_t i = 0; i < 9; ++i)
    {
        if (multiplicity[i] > 0)
        {
            bits_v.push_back(bits[i]);
            loc_v.push_back(locations[i]);
        }
    }

    return {bits_v, loc_v};
}

/**
 * @brief Determines whether a puzzle is still valid by checking whether any entry has value 0.
 *
 * @param goodness
 */
void Puzzle::validPuzzle(bool *goodness)
{
    for (const uint16_t &val : this->data)
    {
        if (val == 0)
        {
            *goodness = false;
        }
    }
}

void Puzzle::printPuzzle(bool nine_bit)
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
        }
        else
        {
            std::cout.flush();
        }
    }
}

void Puzzle::printUnsolved()
{
    for (const uint32_t &index : this->unsolved_indices)
    {
        std::cout << (int)index << " ";
    }

    std::cout << std::endl;
}

void Puzzle::printUGroup(const std::array<std::vector<uint32_t>, 9> &group)
{
    for (uint32_t i = 0; i < 9; ++i)
    {
        for (const uint32_t &elem : group.at(i))
        {
            std::cout << std::setw(2) << (int)elem << " ";
            std::cout.flush();
        }

        std::cout << std::endl;
    }
}

void Puzzle::printRowMap()
{
    for (uint32_t k = 0; k < 81; ++k)
    {
        std::cout << std::setw(2) << (int)k << " -> " << (int)this->refRowIndex(k) << "     ";
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

void Puzzle::printColMap()
{
    for (uint32_t k = 0; k < 81; ++k)
    {
        std::cout << std::setw(2) << (int)k << " -> " << (int)this->refColIndex(k) << "     ";
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

void Puzzle::printBlkMap()
{
    for (uint32_t k = 0; k < 81; ++k)
    {
        std::cout << std::setw(2) << (int)k << " -> " << (int)this->refBlkIndex(k) << "     ";
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