#ifndef SUDOFUN_UTILS_HEADER
#define SUDOFUN_UTILS_HEADER

#include <stdint.h>
#include <string>
#include <stdexcept>
#include <vector>

namespace utils
{
    /**
     * @brief Counts the number of active/hot bits in a number less than or equal to 511.
     *
     * @param n
     * @return uint16_t The number of nonzero bits in the binary representation of the input
     */
    inline uint16_t countBits(uint16_t n)
    {
        uint16_t num = n;
        num = (num & static_cast<uint16_t>(341)) + ((num & static_cast<uint16_t>(170)) >> 1); // 341 = 0b101010101 and 170 = 0b010101010
        num = (num & static_cast<uint16_t>(307)) + ((num & static_cast<uint16_t>(204)) >> 2); // 307 = 0b100110011 and 204 = 0b011001100
        num = (num & static_cast<uint16_t>(271)) + ((num & static_cast<uint16_t>(240)) >> 4); // 271 = 0b100001111 and 240 = 0b011110000
        num = (num & static_cast<uint16_t>(127)) + ((num & static_cast<uint16_t>(256)) >> 8); // 127 = 0b011111111 and 256 = 0b100000000

        return num;
    }

    /**
     * @brief Converts a sudoku cell value to its associated 9 bit value, given by 2**(val - 1)
     *
     * @param val - An integer value from 1 to 9 inclusive.
     * @return uint16_t
     */
    inline uint16_t valueToNineBit(uint32_t val)
    {
        if ((val < 1) && (val > 9))
        {
            throw std::invalid_argument("Input must be in the range 1-9");
        }

        std::string binary_string = "000000000";
        binary_string.replace(9 - val, 1, "1");

        return static_cast<uint16_t>(std::stoi(binary_string, nullptr, 2));
    }

    /**
     * @brief Convert a 9 bit value back to a sudoku cell value (1-9 inclusive). Non-solved cells are 0.
     *
     * @param nb The nine bit value
     * @return uint16_t
     */
    inline uint16_t nineBitToValue(uint16_t nb)
    {
        // Just use a switch instead of doing arithmetic
        switch (nb)
        {
        case 1:
            return 1;
        case 2:
            return 2;
        case 4:
            return 3;
        case 8:
            return 4;
        case 16:
            return 5;
        case 32:
            return 6;
        case 64:
            return 7;
        case 128:
            return 8;
        case 256:
            return 9;
        }
        return 0;
    }

    /**
     * @brief
     *
     * @param src
     * @param ref
     * @return uint16_t
     */
    inline uint16_t bitsInSrcNotInRef(std::vector<uint16_t *> src, std::vector<uint16_t *> ref)
    {
        uint16_t src_bits = 0;
        uint16_t ref_bits = 0;

        // Identify all active bits in source
        for (const auto &src_ptr : src)
        {
            src_bits |= *src_ptr;
        }

        // Identify all active bits in reference
        for (const auto &ref_ptr : ref)
        {
            ref_bits |= *ref_ptr;
        }

        // Determine which are unique to source
        src_bits = src_bits & (ref_bits ^ 511);

        return src_bits;
    }

}

#endif