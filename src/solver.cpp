
#include "solver.hpp"

void Solver::strike(bool *updated)
{
    bool is_done = false;

    while (!is_done)
    {
        uint8_t initial_unsolved = this->puzzle->numUnsolved();

        for (const uint8_t &solved_idx : this->puzzle->latest_solved_indices)
        {
            uint16_t puzzle_value = this->puzzle->getValue(solved_idx);

            // Strike from row groups
            this->puzzle->strikeFromGroup(
                &this->puzzle->row_groups,
                &this->puzzle->row_groups.at(solved_idx),
                solved_idx,
                puzzle_value);

            // Strike from row groups
            this->puzzle->strikeFromGroup(
                &this->puzzle->col_groups,
                &this->puzzle->col_groups.at(solved_idx),
                solved_idx,
                puzzle_value);

            // Strike from row groups
            this->puzzle->strikeFromGroup(
                &this->puzzle->blk_groups,
                &this->puzzle->blk_groups.at(solved_idx),
                solved_idx,
                puzzle_value);
        }

        // Clear the solved indices
        this->puzzle->resetSolvedIndices();

        // Determine which of the unsolved have been solved


        // If we've added any solved indices, we're not done
        is_done = initial_unsolved == this->puzzle->numUnsolved();
    }
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