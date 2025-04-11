
#include "clue.hpp"
#include <iostream>

Clue::Clue(const std::string &clueString)
{
    this->clue_string = this->validateString(clueString);
}

Iterator3 Clue::begin()
{
    return Iterator3(this->clue_string.begin());
}

Iterator3 Clue::end()
{
    return Iterator3(this->clue_string.end());
}

std::string Clue::validateString(std::string clueString)
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