
#ifndef SUDOFUN_CLUE_CLASS_HEADER
#define SUDOFUN_CLUE_CLASS_HEADER

#include <string>
#include <iterator>
#include <stdexcept>
#include <memory>
#include <set>

class Iterator3
{
private:
    std::string::iterator current_;
    size_t step_;

public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::string;
    using difference_type = std::ptrdiff_t;
    using pointer = std::string *;
    using reference = std::string &;

    // Constructor
    Iterator3(std::string::iterator current) : current_(current), step_(3) {}

    // Dereference operator to return a substring of length 3
    reference operator*()
    {
        size_t len = std::min(static_cast<int>(step_), static_cast<int>(std::distance(current_, current_ + step_)));
        return *std::make_shared<std::string>(current_, current_ + len);
    }

    // Pre-increment operator
    Iterator3 &operator++()
    {
        std::advance(current_, step_);
        return *this;
    }

    // Post-increment operator
    Iterator3 operator++(int)
    {
        Iterator3 temp = *this;
        ++(*this);
        return temp;
    }

    // Equality comparison operator
    bool operator==(const Iterator3 &other) const
    {
        return current_ == other.current_;
    }

    // Inequality comparison operator
    bool operator!=(const Iterator3 &other) const
    {
        return !(*this == other);
    }

    const size_t getStep()
    {
        return step_;
    }
};

class Clue
{
private:
    std::string clueString;

public:
    Clue(const std::string &clueString)
    {
        this->clueString = validateString(clueString);
    }

    static std::string validateString(std::string clueString);

    Iterator3 begin() { return Iterator3(clueString.begin()); }
    Iterator3 end() { return Iterator3(clueString.end()); }
};

#endif