
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

/**
 * @brief If an unsolved element has a uniqu bit amongst its row, column, or block, all but the unique
 * bit are dropped and the element is now solved
 *
 * @param updated A flag to track whether the solve stack has resulted in any updates to the puzzle
 */
void Solver::unique(bool *updated)
{
    uint32_t initial_unsolved = this->puzzle->numUnsolved();

    for (const uint32_t &unsolved_idx : this->puzzle->unsolved_indices)
    {
        // First check among the row group
        uint16_t unsolved_val = this->puzzle->getValue(unsolved_idx);

        // Determine which bits do not appear in the row neighbors
        uint16_t unique_bits = this->puzzle->rowNeighborBits(unsolved_idx);
        unique_bits ^= 511; // bin(511) = 111111111

        // Of those missing bits, does our unsolved value contain any of them
        unique_bits &= unsolved_val;

        // If the value is nonzero, then there is a unique bit in our unsolved element. Technically we have
        // only checked that there are any unique bits, but our solver pipeline guarantees that as long as 
        // the clue is valid there will only ever be one unique bit when there are any.
        if (unique_bits != 0)
        {
            this->puzzle->setValue(unsolved_idx, unique_bits);
        }
        else
        {
            // If not a unique among the row, check the column
            unique_bits = this->puzzle->colNeighborBits(unsolved_idx);
            unique_bits ^= 511; // bin(511) = 111111111
            unique_bits &= unsolved_val;
            if (unique_bits != 0)
            {
                this->puzzle->setValue(unsolved_idx, unique_bits);
            }
            else
            {
                // If not a unique among the column, check the block
                unique_bits = this->puzzle->blkNeighborBits(unsolved_idx);
                unique_bits ^= 511; // bin(511) = 111111111
                unique_bits &= unsolved_val;
                if (unique_bits != 0)
                {
                    this->puzzle->setValue(unsolved_idx, unique_bits);
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

/**
 * @brief In every row and column, divide the row/column into three parts according to block membership.
 * If any of the three parts has a unique bit, remove that bit from the remaining members of that part's block.
 *
 * @param updated A flag to track whether the solve stack has resulted in any updates to the puzzle
 */
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