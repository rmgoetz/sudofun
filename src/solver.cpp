
#include "solver.hpp"

void Solver::strike(bool *updated)
{
}

void Solver::unique(bool *updated)
{
}

void Solver::pigeon(bool *updated)
{
}

void Solver::squeeze(bool *updated)
{
}

void Solver::pipe(bool *updated)
{
}

void Solver::solveStack(bool *updated)
{
    strike(updated);
    unique(updated);
    strike(updated);
    squeeze(updated);
    strike(updated);
    pipe(updated);
}

void Solver::reduceLoop()
{

    bool keep_going = true;
    while (keep_going)
    {
        // Run the solver stack and keep going if any of the methods yield a change
        keep_going = false;
        solveStack(&keep_going);

        // Track the number of loop attempts we've made
        this->attempts++;
    }
}

void Solver::reduceLoop(bool *goodness)
{
    bool keep_going = true;
    while (keep_going)
    {
        // Run the solver stack and keep going if any of the methods yield a change
        keep_going = false;
        solveStack(&keep_going);

        // Track the number of loop attempts we've made
        this->attempts++;

        // Perform a validity check of the puzzle
        this->puzzle->validPuzzle(goodness);
        keep_going &= *goodness;
    }
}