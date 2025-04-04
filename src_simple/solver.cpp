
#include "solver.hpp"

void Solver::strike(bool *updated)
{
    bool is_done = false;

    while (!is_done)
    {
        uint32_t initial_unsolved = this->puzzle->numUnsolved();

        // Strike the recently solved from the puzzle
        this->puzzle->strikeLatestFromPuzzle();

        // Update the unsolved and recently solved
        this->postStepUpdate();

        // If we've added any solved indices, we're not done
        is_done = initial_unsolved == this->puzzle->numUnsolved();

        // Track if we've made any update
        *updated |= is_done;
    }
}

void Solver::unique(bool *updated)
{
    uint32_t initial_unsolved = this->puzzle->numUnsolved();

    for (const uint32_t &unsolved_idx : this->puzzle->unsolved_indices)
    {
        // First check among the row group
        uint16_t unsolved_val = this->puzzle->getValue(unsolved_idx);
        uint16_t unq_track = 0;
        for (const uint32_t &neighbor_idx : *this->puzzle->getRowGroup(unsolved_idx))
        {
            if (unsolved_idx != neighbor_idx)
            {
                unq_track |= this->puzzle->getValue(neighbor_idx);
            }
        }
        unq_track ^= 511;
        unq_track &= unsolved_val;

        if (unq_track != 0)
        {
            this->puzzle->setValue(unsolved_idx, unq_track);
        }
        else
        {
            // If not a unique among the row, check the column
            unq_track = 0;
            for (const uint32_t &neighbor_idx : *this->puzzle->getColGroup(unsolved_idx))
            {
                if (unsolved_idx != neighbor_idx)
                {
                    unq_track |= this->puzzle->getValue(neighbor_idx);
                }
            }
            unq_track ^= 511;
            unq_track &= unsolved_val;

            if (unq_track != 0)
            {
                this->puzzle->setValue(unsolved_idx, unq_track);
            }
            else
            {
                // If not a unique among the column, check the block
                unq_track = 0;
                for (const uint32_t &neighbor_idx : *this->puzzle->getBlkGroup(unsolved_idx))
                {
                    if (unsolved_idx != neighbor_idx)
                    {
                        unq_track |= this->puzzle->getValue(neighbor_idx);
                    }
                }
                unq_track ^= 511;
                unq_track &= unsolved_val;

                if (unq_track != 0)
                {
                    this->puzzle->setValue(unsolved_idx, unq_track);
                }
            }
        }
    }

    // Update the unsolved and recently solved
    this->postStepUpdate();

    // Track if we've made any update
    *updated |= initial_unsolved == this->puzzle->numUnsolved();
}

void Solver::pigeon(bool *updated)
{
    for (uint32_t i = 0; i < 9; ++i)
    {
    }

    for (uint32_t j = 0; j < 9; ++j)
    {
    }

    for (uint32_t g = 0; g < 9; ++g)
    {
    }

    this->postStepUpdate();
}

void Solver::squeeze(bool *updated)
{
    for (uint32_t i = 0; i < 9; ++i)
    {
    }

    for (uint32_t j = 0; j < 9; ++j)
    {
    }

    this->postStepUpdate();
}

void Solver::pipe(bool *updated)
{
    for (uint32_t g = 0; g < 9; ++g)
    {
    }

    this->postStepUpdate();
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

void Solver::postStepUpdate()
{

    // Clear the solved indices
    this->puzzle->resetSolvedIndices();

    // Determine which of the unsolved have been solved, and pop those that
    // have into the most recently solved
    this->puzzle->checkUnsolved();
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