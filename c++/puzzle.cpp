
#include "puzzle.hpp"
#include "utils.hpp"

Puzzle::Puzzle() : data({0})
{
}

void Puzzle::setValue(uint8_t index, uint16_t val)
{
    data[index] = val;
}

uint16_t Puzzle::getValue(uint8_t index)
{
    return data[index];
}

void Puzzle::addClueString(Clue clue)
{

    for (auto i = clue.begin(); i != clue.end(); ++i)
    {
        // 
        std::string s = *i;
        
        //
        uint8_t row_idx = static_cast<uint8_t>(s[0] - '0');
        uint8_t col_idx = static_cast<uint8_t>(s[1] - '0');
        uint8_t val_num = static_cast<uint8_t>(s[2] - '0');

        // Convert the value number to a 9-bit value
        this->setValue(row_idx, valueToNineBit(val_num));

    }
}