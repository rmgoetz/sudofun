
#ifndef SUDOFUN_SOLVER_CLASS_HEADER
#define SUDOFUN_SOLVER_CLASS_HEADER

#include "puzzle.hpp"

class Solver
{
private:
    Puzzle *puzzle;
    int total_loops;
    int guesses;

public:
    Solver(Puzzle *puzzle) : puzzle(puzzle), total_loops(0), guesses(0) {}

    // Solver rules
    void strike(bool *updated);
    void unique(bool *updated);
    void pigeon(bool *updated);
    void squeeze(bool *updated);
    void pipe(bool *updated);
    void solveStack(bool *updated);
    void postStepUpdate();

    // An iterative deterministic solver loop
    void reduceLoop();
    void reduceLoop(bool *goodness);

    // Guesser solver
    void guessAndCheck();

    // 
    void solve(int max_guesses = 100);
};

#endif