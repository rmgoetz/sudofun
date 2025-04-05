
#include "puzzle.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <stdio.h>

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

    //
    unsolved_indices = INIT_UNSOLVED_INDICES;
    row_u_groups = INIT_ROW_GROUPS;
    col_u_groups = INIT_COL_GROUPS;
    blk_u_groups = INIT_BLK_GROUPS;
}

/**
 * @brief
 *
 * @param clue
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

uint32_t Puzzle::numUnsolved()
{
    return this->unsolved_indices.size();
}

const uint32_t &Puzzle::refRowIndex(const uint32_t &flat_index)
{
    return this->flat_to_row_col_block->at(flat_index).at(0);
}

const uint32_t &Puzzle::refColIndex(const uint32_t &flat_index)
{
    return this->flat_to_row_col_block->at(flat_index).at(1);
}

const uint32_t &Puzzle::refBlkIndex(const uint32_t &flat_index)
{
    return this->flat_to_row_col_block->at(flat_index).at(2);
}

const std::array<uint32_t, 9> &Puzzle::refIndicesInRow(const uint32_t &row_index)
{
    return row_to_flat->at(row_index);
}

const std::array<uint32_t, 9> &Puzzle::refIndicesInCol(const uint32_t &col_index)
{
    return col_to_flat->at(col_index);
}

const std::array<uint32_t, 9> &Puzzle::refIndicesInBlk(const uint32_t &blk_index)
{
    return blk_to_flat->at(blk_index);
}

std::vector<uint32_t> *Puzzle::ptrRowUGroup(const uint32_t &flat_idx)
{
    return &this->row_u_groups.at(this->refRowIndex(flat_idx));
}

std::vector<uint32_t> *Puzzle::ptrColUGroup(const uint32_t &flat_idx)
{
    return &this->col_u_groups.at(this->refColIndex(flat_idx));
}

std::vector<uint32_t> *Puzzle::ptrBlkUGroup(const uint32_t &flat_idx)
{
    return &this->blk_u_groups.at(this->refBlkIndex(flat_idx));
}

std::array<uint32_t, 9> *Puzzle::ptrRowInCol(const uint32_t &col_index)
{
    return const_cast<std::array<uint32_t, 9> *>(&col_to_row->at(col_index));
}

std::array<uint32_t, 9> *Puzzle::ptrColInRow(const uint32_t &row_index)
{
    return const_cast<std::array<uint32_t, 9> *>(&row_to_col->at(row_index));
}

std::array<uint32_t, 3> *Puzzle::ptrRowInBlk(const uint32_t &blk_index)
{
    return const_cast<std::array<uint32_t, 3> *>(&blk_to_row->at(blk_index));
}

std::array<uint32_t, 3> *Puzzle::ptrColInBlk(const uint32_t &blk_index)
{
    return const_cast<std::array<uint32_t, 3> *>(&blk_to_col->at(blk_index));
}

std::array<uint32_t, 3> *Puzzle::ptrBlkInRow(const uint32_t &row_index)
{
    return const_cast<std::array<uint32_t, 3> *>(&row_to_blk->at(row_index));
}

std::array<uint32_t, 3> *Puzzle::ptrBlkInCol(const uint32_t &col_index)
{
    return const_cast<std::array<uint32_t, 3> *>(&col_to_blk->at(col_index));
}

const std::array<uint32_t, 9> &Puzzle::refRowInCol(const uint32_t &col_index)
{
    return col_to_row->at(col_index);
}

const std::array<uint32_t, 9> &Puzzle::refColInRow(const uint32_t &row_index)
{
    return row_to_col->at(row_index);
}

const std::array<uint32_t, 3> &Puzzle::refRowInBlk(const uint32_t &blk_index)
{
    return blk_to_row->at(blk_index);
}

const std::array<uint32_t, 3> &Puzzle::refColInBlk(const uint32_t &blk_index)
{
    return blk_to_col->at(blk_index);
}

const std::array<uint32_t, 3> &Puzzle::refBlkInRow(const uint32_t &row_index)
{
    return row_to_blk->at(row_index);
}

const std::array<uint32_t, 3> &Puzzle::refBlkInCol(const uint32_t &col_index)
{
    return col_to_blk->at(col_index);
}

std::array<uint16_t *, 9> Puzzle::ptrValuesInRow(const uint32_t &row_index)
{
    std::array<uint16_t *, 9> vals_in_row;

    for (int n = 0; n < 9; ++n)
    {
        vals_in_row[n] = this->ptrValue(this->refIndicesInRow(row_index).at(n));
    }

    return vals_in_row;
}

std::array<uint16_t *, 9> Puzzle::ptrValuesInCol(const uint32_t &col_index)
{
    std::array<uint16_t *, 9> vals_in_col;

    for (int n = 0; n < 9; ++n)
    {
        vals_in_col[n] = this->ptrValue(this->refIndicesInCol(col_index).at(n));
    }

    return vals_in_col;
}

std::array<uint16_t *, 9> Puzzle::ptrValuesInBlk(const uint32_t &blk_index)
{
    std::array<uint16_t *, 9> vals_in_blk;

    for (int n = 0; n < 9; ++n)
    {
        vals_in_blk[n] = this->ptrValue(this->refIndicesInBlk(blk_index).at(n));
    }

    return vals_in_blk;
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

void Puzzle::removeFromUGroups(const std::vector<uint32_t> &cut_vector)
{
    for (const uint32_t &cut_index : cut_vector)
    {
        this->removeFromUGroups(cut_index);
    }
}

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
 * @brief For a solved cell specified by index, remove the bit from all of its groups and then remove
 * it from every group.
 *
 * @param strike_idx
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

void Puzzle::strikeFromPuzzle(const std::vector<uint32_t> &strike_idx_vec)
{
    for (const uint32_t &strike_idx : strike_idx_vec)
    {
        this->strikeFromPuzzle(strike_idx);
    }
}

void Puzzle::strikeLatestFromPuzzle()
{
    this->strikeFromPuzzle(this->latest_solved_indices);
}

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
    for (int i = 0; i < 3; ++i)
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

uint16_t Puzzle::rowNeighborBits(const uint32_t &index)
{
    uint16_t neighbor_bits = 0;
    // Accumulate all of the bits found amongst row neighbors
    for (const uint32_t &neighbor_idx : *this->ptrRowUGroup(index))
    {
        if (index != neighbor_idx)
        {
            neighbor_bits |= this->getValue(neighbor_idx);
        }
    }

    return neighbor_bits;
}

uint16_t Puzzle::colNeighborBits(const uint32_t &index)
{
    uint16_t neighbor_bits = 0;
    // Accumulate all of the bits found amongst col neighbors
    for (const uint32_t &neighbor_idx : *this->ptrColUGroup(index))
    {
        if (index != neighbor_idx)
        {
            neighbor_bits |= this->getValue(neighbor_idx);
        }
    }

    return neighbor_bits;
}

uint16_t Puzzle::blkNeighborBits(const uint32_t &index)
{
    uint16_t neighbor_bits = 0;
    // Accumulate all of the bits found amongst blk neighbors
    for (const uint32_t &neighbor_idx : *this->ptrBlkUGroup(index))
    {
        if (index != neighbor_idx)
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
 * @return std::tuple<uint32_t, uint32_t, std::vector<uint32_t>> The bit index, multiplicity
 */
std::tuple<uint32_t, uint32_t, std::vector<uint32_t>> Puzzle::leastPopularBit()
{
}

void Puzzle::validPuzzle(bool *goodness)
{
}

void Puzzle::printPuzzle()
{
    for (int i = 0; i < 81; ++i)
    {
        std::cout << std::setw(3) << this->getValue(i) << " ";
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
    for (int i = 0; i < 9; ++i)
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