
#include "puzzle.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>

Puzzle::Puzzle()
{
    // Initialize the puzzle data to be entirely unsolved
    data.fill(511); // bin(511) = 0000000111111111

    // Signify that we haven't loaded any clue yet
    loaded_clue = false;

    // Build the row/col/blk groups
    for (int k = 0; k < 81; ++k)
    {
        // Add index to unsolved list
        this->unsolved_indices.push_back(k);

        // The row/col/blk index for the kth (flat) puzzle element
        uint32_t i = k / 9;
        uint32_t j = k % 9;
        uint32_t g = 3 * (i / 3) + (j / 3);

        // Populate the row/col/blk groups
        this->row_groups.at(i).push_back(k);
        this->col_groups.at(j).push_back(k);
        this->blk_groups.at(g).push_back(k);

        // Populate the index -> group maps
        this->row_map[k] = i;
        this->col_map[k] = j;
        this->blk_map[k] = g;
    }
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

    this->removeFromGroups(this->latest_solved_indices);
}

void Puzzle::setValue(uint32_t index, uint16_t val)
{
    data.at(index) = val;
}

uint16_t Puzzle::getValue(uint32_t index)
{
    return data.at(index);
}

uint32_t Puzzle::numUnsolved()
{
    return this->unsolved_indices.size();
}

/**
 * @brief
 *
 * @param group
 * @param cut_value
 */
void removeFromGroup(std::vector<uint32_t> *group, const uint32_t &cut_value)
{
    // If the cut index is in the group, remove it (there will be at most one instance).
    // NOTE: using std::find instead of this loop fails to compile on my build system
    for (auto it = group->begin(); it != group->end(); ++it)
    {
        if (*it == cut_value)
        {
            group->erase(it);
            break;
        }
    }
}

void Puzzle::removeFromGroups(const uint32_t &cut_value)
{
    // Convert to row, col, and blk indices
    uint32_t row_idx = this->row_map[cut_value];
    uint32_t col_idx = this->col_map[cut_value];
    uint32_t blk_idx = this->blk_map[cut_value];

    // If the cut index is in the group, remove it (there will be at most one instance).
    // NOTE: using std::find instead of this loop fails to compile on my build system
    removeFromGroup(&this->row_groups.at(row_idx), cut_value);
    removeFromGroup(&this->col_groups.at(col_idx), cut_value);
    removeFromGroup(&this->blk_groups.at(blk_idx), cut_value);
}

void Puzzle::removeFromGroups(const std::vector<uint32_t> &cut_vector)
{
    for (const uint32_t &cut_index : cut_vector)
    {
        this->removeFromGroups(cut_index);
    }
}

void Puzzle::bitRemoveFromGroup(std::vector<uint32_t> *group, const uint32_t &strike_idx, const uint16_t &strike_val)
{
    uint32_t remove_offset = 0;
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
    group->erase(group->begin() + remove_offset);
}

/**
 * @brief For a solved cell specified by index, remove the bit from all of its groups and then remove
 * it from every group.
 *
 * @param strike_idx
 */
void Puzzle::strikeFromPuzzle(const uint32_t &strike_idx)
{
    uint32_t row_idx = this->row_map[strike_idx];
    uint32_t col_idx = this->col_map[strike_idx];
    uint32_t blk_idx = this->blk_map[strike_idx];
    uint16_t puzzle_value = this->getValue(strike_idx);

    

    std::cout << row_idx << " " << col_idx << " " << blk_idx << std::endl;
    std::cout << puzzle_value << std::endl;

    this->bitRemoveFromGroup(&this->row_groups.at(row_idx), strike_idx, puzzle_value);
    this->bitRemoveFromGroup(&this->col_groups.at(col_idx), strike_idx, puzzle_value);
    this->bitRemoveFromGroup(&this->blk_groups.at(blk_idx), strike_idx, puzzle_value);
}

void Puzzle::strikeFromPuzzle(const std::vector<uint32_t> &strike_idx_vec)
{
    for (const uint32_t &strike_idx : strike_idx_vec)
    {
        std::cout << "strike idx: " << strike_idx << std::endl;
        this->strikeFromPuzzle(strike_idx);
    }
}

void Puzzle::strikeLatestFromPuzzle()
{
    this->strikeFromPuzzle(this->latest_solved_indices);
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

void Puzzle::printGroup(const std::array<std::vector<uint32_t>, 9> &group)
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
        std::cout << (int)k << "->" << (int)this->row_map[k] << "     ";
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
        std::cout << (int)k << "->" << (int)this->col_map[k] << "     ";
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
        std::cout << (int)k << "->" << (int)this->blk_map[k] << "     ";
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