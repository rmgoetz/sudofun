
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
    StringClue(const std::string &clueString);
    static std::string validateString(std::string clueString);
};

class WindowClue
{
    using VectorTuple = std::vector<std::tuple<uint32_t, uint32_t>>;

public:
    VectorTuple clue_vector;
    WindowClue(const VectorTuple &vectorClue);
    static VectorTuple validateVector(VectorTuple vectorClue);
};

#endif