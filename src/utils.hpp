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
        constexpr uint16_t u_0b101010101{341}; // 341 = 0b101010101
        constexpr uint16_t u_0b010101010{170}; // 170 = 0b010101010
        constexpr uint16_t u_0b100110011{307}; // 307 = 0b100110011
        constexpr uint16_t u_0b011001100{204}; // 204 = 0b011001100
        constexpr uint16_t u_0b100001111{271}; // 271 = 0b100001111
        constexpr uint16_t u_0b011110000{240}; // 240 = 0b011110000
        constexpr uint16_t u_0b011111111{127}; // 127 = 0b011111111
        constexpr uint16_t u_0b100000000{256}; // 256 = 0b100000000

        uint16_t num = n;

        num = (num & u_0b101010101) + ((num & u_0b010101010) >> 1);
        num = (num & u_0b100110011) + ((num & u_0b011001100) >> 2);
        num = (num & u_0b100001111) + ((num & u_0b011110000) >> 4);
        num = (num & u_0b011111111) + ((num & u_0b100000000) >> 8);

        return num;
    }

    /**
     * @brief Returns 0 if n has 0 or 1 hot bits in the first 9 bits. We use the fact that in binary
     * representation, if more than two bits are nonzero, then adding with 511 (first 9 bits hot) will
     * leave all but the lowest hot bit unchanged. For example:
     *      
     *      111111111
     *    + 001001000
     *      ---------
     *     1001000111
     * 
     * @param n
     * @return uint16_t
     */
    inline uint16_t zeroIfOneOrNoBits(uint16_t n)
    {
        constexpr uint16_t u_0_thru_9_hot{511};
        return (u_0_thru_9_hot + n) & n;
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
     * @brief Returns the active bits in the src that are not in ref
     *
     * @param src
     * @param ref
     * @return uint16_t
     */
    inline uint16_t bitsInSrcNotInRef(const std::vector<uint16_t *> &src, const std::vector<uint16_t *> &ref)
    {
        uint16_t src_bits = 0;
        uint16_t ref_bits = 0;

        // Identify all active bits in source
        for (const auto &src_ptr : src)
        {
            src_bits |= *src_ptr;
        }

        // Remove any bits in the ref
        for (const auto &ref_ptr : ref)
        {
            src_bits &= (*ref_ptr ^ 511);
        }

        return src_bits;
    }

}

#endif