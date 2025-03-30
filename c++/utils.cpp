/* A test of the countbits function */

#include "utils.hpp"
#include <string>

namespace utils
{

	uint16_t countBits(uint16_t n)
	{
		uint16_t num = n;
		num = (num & static_cast<uint16_t>(341)) + ((num & static_cast<uint16_t>(170)) >> 1); // 341 = 0b101010101 and 170 = 0b010101010
		num = (num & static_cast<uint16_t>(307)) + ((num & static_cast<uint16_t>(204)) >> 2); // 307 = 0b100110011 and 204 = 0b011001100
		num = (num & static_cast<uint16_t>(271)) + ((num & static_cast<uint16_t>(240)) >> 4); // 271 = 0b100001111 and 240 = 0b011110000
		num = (num & static_cast<uint16_t>(127)) + ((num & static_cast<uint16_t>(256)) >> 8); // 127 = 0b011111111 and 256 = 0b100000000

		return num;
	}

	uint16_t valueToNineBit(uint8_t val)
	{
		std::string binary_string = "000000000";
		binary_string[9 - val] = '1';
		return static_cast<uint16_t>(std::stoi(binary_string, nullptr, 2));
	}
} // namespace utils
