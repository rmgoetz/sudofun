
#include "clue.hpp"

std::string Clue::validateString(std::string clueString)
{

    std::string return_string;
    int str_len = clueString.length();

    std::set<char> charSet = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

    // The first validity check is just that we have the correct length
    if ((str_len + 1) % 4 != 0)
    {
        throw std::invalid_argument("Clue string is improperly formatted");
    }

    for (int i = 0; i < str_len; ++i)
    {
        char c = clueString[i];

        // Index values that are multiples of three should be colon separators
        if ((i % 3 == 0) && (c != ':'))
        {
            throw std::invalid_argument("Clue string is improperly formatted");
        }
        else if (charSet.find(c) != charSet.end())
        {
            return_string.append(1, c);
        }
        else
        {
            throw std::invalid_argument("Clue string is improperly formatted");
        }
    }

    return return_string;
}