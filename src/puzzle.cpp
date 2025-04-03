
#include "puzzle.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>

Puzzle::Puzzle()
{
    // Initialize the puzzle data to be entirely unsolved
    data.fill(511); // bin(511) = 0000000111111111

    // Build the row/col/blk groups
    for (int k = 0; k < 81; ++k)
    {
        // Add index to unsolved list
        this->unsolved_indices.push_back(k);

        // The row/col/blk index for the kth (flat) puzzle element
        uint8_t i = k / 9;
        uint8_t j = k % 9;
        uint8_t g = 3 * (i / 3) + (j / 3);

        for (int n = 0; n < 9; ++n)
        {
            // Enumerate every member of the row/col/blk in flat indices
            uint8_t k_in = 9 * i + n;
            uint8_t k_jn = 9 * n + j;
            uint8_t k_gn = 9 * (3 * (g / 3) + (n / 3)) + 3 * (g % 3) + (n % 3);

            if (k_in != k)
            {
                this->row_groups[k].push_back(k_in);
            }

            if (k_jn != k)
            {
                this->col_groups[k].push_back(k_jn);
            }

            if (k_gn != k)
            {
                this->blk_groups[k].push_back(k_gn);
            }
        }
    }
}

void Puzzle::setValue(uint8_t index, uint16_t val)
{
    data.at(index) = val;
}

uint16_t Puzzle::getValue(uint8_t index)
{
    return data.at(index);
}

uint8_t Puzzle::numUnsolved()
{
    return this->unsolved_indices.size();
}

void Puzzle::removeFromGroup(std::vector<uint8_t> *group_vec, const uint8_t &cut_index)
{
    // If the cut index is in the group, remove it (there will be at most one instance).
    // NOTE: using std::find instead of this loop fails to compile on my build system
    for (auto it = group_vec->begin(); it != group_vec->end(); ++it)
    {
        if (*it == cut_index)
        {
            group_vec->erase(it);
            break;
        }
    }
}

void Puzzle::strikeFromGroup(
    std::array<std::vector<uint8_t>, 81> *group,
    std::vector<uint8_t> *group_vec,
    const uint8_t &cut_index,
    const uint16_t &ref_val)
{
    //
    for (const uint8_t &neighbor_idx : *group_vec)
    {
        this->data[neighbor_idx] -= this->data[neighbor_idx] & ref_val;
        this->removeFromGroup(&group->at(neighbor_idx), cut_index);
    }
}

void Puzzle::removeSolvedFromGroups()
{
    // Loop over the latest solved indices
    for (const uint8_t &solved_idx : this->latest_solved_indices)
    {
        uint16_t puzzle_value = this->getValue(solved_idx);

        // Remove from the
        this->removeFromGroup(&this->row_groups.at(solved_idx), solved_idx);
        this->removeFromGroup(&this->col_groups.at(solved_idx), solved_idx);
        this->removeFromGroup(&this->blk_groups.at(solved_idx), solved_idx);
    }
}

void Puzzle::resetSolvedIndices()
{
    this->latest_solved_indices.clear();
}

void checkUnsolved()
{
    // Check
}

void Puzzle::addClueString(Clue *clue)
{

    for (auto i = clue->begin(); i != clue->end(); ++i)
    {
        //
        std::string s = *i;

        // Convert clues to their index/value combination, remembering that cluestrings will
        // index starting at 1, not zero
        uint8_t row_idx = static_cast<uint8_t>(s[0] - '0') - 1;
        uint8_t col_idx = static_cast<uint8_t>(s[1] - '0') - 1;
        uint8_t val_num = static_cast<uint8_t>(s[2] - '0');
        uint8_t flat_index = row_idx * 9 + col_idx;

        // Set the corresponding flat index to the value converted to a 9-bit representation
        this->setValue(flat_index, utils::valueToNineBit(val_num));

        // Add to the list of solved and remove from unsolved
    }

    this->removeSolvedFromGroups();
}

/**
 * @brief
 *
 * @return std::tuple<uint8_t, uint8_t, std::vector<uint8_t>> The bit index, multiplicity
 */
std::tuple<uint8_t, uint8_t, std::vector<uint8_t>> Puzzle::leastPopularBit()
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