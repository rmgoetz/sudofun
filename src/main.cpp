/* An attempt to put everything together    */

#include "clue.hpp"
#include "puzzle.hpp"
#include "utils.hpp"
#include <iostream>
#include <tuple>
#include <list>
#include <array>

int main()
{

    int in_num;
    std::cout << "Enter a number less than or equal to 511: ";
    std::cin >> in_num;
    std::cout << "There are " << utils::countBits(in_num) << " hot bits in the number " << in_num << std::endl;

    int in_small_num;
    std::cout << "Enter a number in the range 1-9: ";
    std::cin >> in_small_num;
    std::cout << in_small_num << " converts to " << utils::valueToNineBit(in_small_num) << std::endl;

    std::string innie;
    std::cout << "Enter a clue string: ";
    std::cin >> innie;
    Clue clue = Clue(innie);

    return 0;
}
