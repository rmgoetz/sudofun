/* An attempt to put everything together    */

#include "clue.hpp"
#include "puzzle.hpp"
#include "solver.hpp"
#include "utils.hpp"
#include <iostream>
#include <tuple>
#include <list>
#include <array>

int main()
{
    std::string input_cluestring;
    std::cout << "Enter a clue string for simple src: ";
    std::cin >> input_cluestring;
    std::cout.flush();
    std::cout << "\n\n" << std::endl;


    Clue clue = Clue(input_cluestring);
    Puzzle puzzle = Puzzle();
    puzzle.addClueString(&clue);
    puzzle.printPuzzle(false);
    std::cout << "\n\n" << std::endl;
    std::cout.flush();

    Solver solver = Solver(&puzzle);
    solver.solve(1);

    puzzle.printPuzzle(false);

    return 0;
}
