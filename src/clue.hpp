
#ifndef SUDOFUN_CLUE_CLASS_HEADER
#define SUDOFUN_CLUE_CLASS_HEADER

#include <string>
#include <iterator>
#include <stdexcept>
#include <memory>
#include <set>
#include <vector>

class StringClue
{

public:
    std::string clue_string;

    StringClue(const std::string &clueString) : clue_string(validateString(clueString)) {}

    std::string validateString(std::string clueString)
    {

        std::string return_string;
        int str_len = clueString.length();

        std::set<char> charSet = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

        // The first validity check is just that we have the correct length
        if ((str_len + 1) % 4 != 0)
        {
            throw std::invalid_argument("Clue string is improperly formatted: wrong length");
        }

        for (int i = 0; i < str_len; ++i)
        {
            char c = clueString[i];

            // Index values that are multiples of three should be colon separators
            if ((i + 1) % 4 == 0)
            {
                if (c != ':')
                {
                    throw std::invalid_argument("Clue string is improperly formatted: no colon delimiters");
                }
            }
            else if (charSet.find(c) != charSet.end())
            {
                return_string.append(1, c);
            }
            else
            {
                throw std::invalid_argument("Clue string is improperly formatted: illegal characters");
            }
        }

        return return_string;
    }
};

class WindowClue
{
    using VectorTuple = std::vector<std::tuple<uint32_t, uint32_t>>;

public:
    VectorTuple clue_vector;

    WindowClue(const VectorTuple &vectorClue)
    {
        this->clue_vector = this->validateVector(vectorClue);
    }

    static VectorTuple validateVector(VectorTuple vectorClue)
    {
        // Every input must be between 1 and 9 inclusive
        for (auto i : vectorClue)
        {
            uint32_t val = std::get<1>(i);
            if ((val < 1) || (val > 9))
            {
                throw std::invalid_argument("Sudoku values must fall between 1 and 9, inclusive");
            }
        }

        return vectorClue;
    }
};

#endif