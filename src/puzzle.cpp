
#include "puzzle.hpp"
#include "utils.hpp"

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

        // Set the corresponding flat index to the value converted to a 9-bit representation
        this->setValue(row_idx * 9 + col_idx, utils::valueToNineBit(val_num));
    }
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