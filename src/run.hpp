
#ifndef SUDOFUN_RUN_SOLVERS_HEADER
#define SUDOFUN_RUN_SOLVERS_HEADER

#if defined(BUILT_WITH_QT5)

#include <QApplication>
#include "window.hpp"

#endif

#include "solver.hpp"
#include <iostream>
#include <tuple>
#include <list>
#include <array>

void runConsoleSolve(int maxGuesses, bool nine_bit_print = false)
{
    std::string input_cluestring;
    std::cout << "Enter a clue string for simple src: ";
    std::cin >> input_cluestring;
    std::cout.flush();
    std::cout << "\n\n"
              << std::endl;

    StringClue clue = StringClue(input_cluestring);
    Puzzle puzzle = Puzzle();
    puzzle.addClueString(&clue);
    puzzle.printPuzzle(false);
    std::cout << "\n\n"
              << std::endl;
    std::cout.flush();

    Solver solver = Solver(&puzzle);
    solver.solve(maxGuesses);

    puzzle.printPuzzle(nine_bit_print);
}

#if defined(BUILT_WITH_QT5)

int runGuiSolve(int maxGuesses, int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window(nullptr, maxGuesses);
    window.show();
    return app.exec();
}

#endif
#endif