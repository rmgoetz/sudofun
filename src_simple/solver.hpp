
#ifndef SOLVER_CLASS_HEADER
#define SOLVER_CLASS_HEADER

#include "puzzle.hpp"

class Solver
{
private:
    Puzzle *puzzle;
    int attempts;

public:
    Solver(Puzzle *puzzle) : puzzle(puzzle), attempts(0) {}

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
};

#endif