
#include "solver.hpp"
#include "puzzle.hpp"
#include "utils.hpp"

/**
 * @brief Run through the most recently solved values and remove the corresponding bits from the rest of
 * their row, column, and block.
 *
 * @param puzzle
 * @param updated A flag to track whether the solve stack has resulted in any updates to the puzzle
 */
void Solver::strike(Puzzle *puzzle, bool *updated)
{
    bool is_done = false;

    while (!is_done)
    {
        uint32_t initial_unsolved = puzzle->numUnsolved();

        // Strike the recently solved from the puzzle
        puzzle->strikeLatestFromPuzzle();

        // Update the unsolved and recently solved
        this->postStepUpdate(puzzle);

        // If we've added any solved indices, we're not done
        is_done = initial_unsolved == puzzle->numUnsolved();

        // Track if we've made any update
        *updated |= !is_done;
    }
}

/**
 * @brief If an unsolved element has a unique bit amongst its row, column, or block, all but the unique
 * bit are dropped and the element is now solved
 *
 * @param puzzle
 * @param updated A flag to track whether the solve stack has resulted in any updates to the puzzle
 */
void Solver::unique(Puzzle *puzzle, bool *updated)
{
    uint32_t initial_unsolved = puzzle->numUnsolved();

    for (const uint32_t &unsolved_idx : puzzle->unsolved_indices)
    {
        // First check among the row group
        uint16_t unsolved_val = puzzle->getValue(unsolved_idx);

        // Determine which bits do not appear in the row neighbors
        uint16_t unique_bits = puzzle->rowNeighborBits(unsolved_idx);
        unique_bits ^= 511; // bin(511) = 111111111

        // Of those missing bits, does our unsolved value contain any of them
        unique_bits &= unsolved_val;

        // If the value is nonzero, then there is a unique bit in our unsolved element. Technically we have
        // only checked that there are any unique bits, but our solver pipeline guarantees that as long as
        // the clue is valid there will only ever be one unique bit when there are any.
        if (unique_bits != 0)
        {
            puzzle->setValue(unsolved_idx, unique_bits);
        }
        else
        {
            // If not a unique among the row, check the column
            unique_bits = puzzle->colNeighborBits(unsolved_idx);
            unique_bits ^= 511; // bin(511) = 111111111
            unique_bits &= unsolved_val;
            if (unique_bits != 0)
            {
                puzzle->setValue(unsolved_idx, unique_bits);
            }
            else
            {
                // If not a unique among the column, check the block
                unique_bits = puzzle->blkNeighborBits(unsolved_idx);
                unique_bits ^= 511; // bin(511) = 111111111
                unique_bits &= unsolved_val;
                if (unique_bits != 0)
                {
                    puzzle->setValue(unsolved_idx, unique_bits);
                }
            }
        }
    }

    // Update the unsolved and recently solved
    postStepUpdate(puzzle);

    // Track if we've made any update
    *updated |= initial_unsolved != puzzle->numUnsolved();
}

void Solver::pigeon(Puzzle *puzzle, bool *updated)
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

    this->postStepUpdate(puzzle);
}

/**
 * @brief In every row and column, divide the row/column into three parts according to block membership.
 * If any of the three parts has a unique bit, remove that bit from the remaining members of that part's block.
 *
 * @param puzzle
 * @param updated A flag to track whether the solve stack has resulted in any updates to the puzzle
 */
void Solver::squeeze(Puzzle *puzzle, bool *updated)
{
    uint32_t initial_unsolved = puzzle->numUnsolved();

    // Values to track unique bits
    std::array<uint16_t, 3> unique_bits;
    uint16_t unique;

    // Loop over the rows
    for (uint32_t i = 0; i < 9; ++i)
    {
        // Identify bits unique to each section, if any
        unique_bits = puzzle->uniqueBitsInSections(i, true);

        // Loop over each of the three sections of the row
        for (uint32_t n = 0; n < 3; ++n)
        {
            unique = unique_bits.at(n);

            // If there are unique bits in a section, remove them from the unsolved non-row members of the blk
            if (unique != 0)
            {
                for (uint16_t *gval : puzzle->uBlkValuesNotInRow(3 * (i / 3) + n, i))
                {
                    *gval -= *gval & unique;
                }
            }
        }
    }

    // Loop over the cols
    for (uint32_t j = 0; j < 9; ++j)
    {
        // Identify bits unique to each section, if any
        unique_bits = puzzle->uniqueBitsInSections(j, false);

        // Loop over each of the three sections of the col
        for (uint32_t n = 0; n < 3; ++n)
        {
            unique = unique_bits.at(n);

            // If there are unique bits in a section, remove them from the non-col members of the blk
            if (unique != 0)
            {
                for (uint16_t *gval : puzzle->uBlkValuesNotInCol(j / 3 + 3 * n, j))
                {
                    *gval -= *gval & unique;
                }
            }
        }
    }

    // Update the unsolved and recently solved
    this->postStepUpdate(puzzle);

    // Track if we've made any update
    *updated |= initial_unsolved != puzzle->numUnsolved();
}

/**
 * @brief In every block select the unsolved elements and check if any bit is unique to a row or column within
 * the block. For each unique bit eliminate them from row/column members outside if the block.
 *
 * @param puzzle
 * @param updated A flag to track whether the solve stack has resulted in any updates to the puzzle
 */
void Solver::pipe(Puzzle *puzzle, bool *updated)
{
    uint32_t initial_unsolved = puzzle->numUnsolved();

    // Loop over the blocks
    for (uint32_t g = 0; g < 9; ++g)
    {
        // Loop over the rows/cols of the block
        for (uint32_t n = 0; n < 9; ++n)
        {
            // Grab the unsolved elements of the block, split into which are part of the row/col, and which are not
            auto [blk_in_row, blk_not_in_row] = puzzle->uBlkValuesSiftRow(g, n);
            auto [blk_in_col, blk_not_in_col] = puzzle->uBlkValuesSiftCol(g, n);

            // Determine if there are any unique bits of those block members in the row/col
            uint16_t unique_row_bits = utils::bitsInSrcNotInRef(blk_in_row, blk_not_in_row);
            uint16_t unique_col_bits = utils::bitsInSrcNotInRef(blk_in_col, blk_not_in_col);

            // Remove those bits from non-block row/col members
            if (unique_row_bits != 0)
            {
                for (uint16_t *gval : puzzle->uRowValuesNotInBlk(n, g))
                {
                    *gval -= *gval & unique_row_bits;
                }
            }
            if (unique_col_bits != 0)
            {
                for (uint16_t *gval : puzzle->uColValuesNotInBlk(n, g))
                {
                    *gval -= *gval & unique_col_bits;
                }
            }
        }
    }

    // Update the unsolved and recently solved
    this->postStepUpdate(puzzle);

    // Track if we've made any update
    *updated |= initial_unsolved != puzzle->numUnsolved();
}

void Solver::solveStack(Puzzle *puzzle, bool *updated)
{
    strike(puzzle, updated);
    unique(puzzle, updated);
    strike(puzzle, updated);
    squeeze(puzzle, updated);
    strike(puzzle, updated);
    pipe(puzzle, updated);
}

void Solver::postStepUpdate(Puzzle *puzzle)
{
    // Clear the solved indices
    puzzle->resetSolvedIndices();

    // Determine which of the unsolved have been solved, and pop those that
    // have into the most recently solved
    puzzle->checkUnsolved();
}

void Solver::reduceLoop(Puzzle *puzzle)
{
    bool keep_going = true;
    while (keep_going)
    {
        // Run the solver stack and keep going if any of the methods yield a change
        keep_going = false;
        solveStack(puzzle, &keep_going);

        // Track the number of loop attempts we've made
        this->total_loops++;
    }

    // Run one more strike for cleanup after the loop
    strike(puzzle, &keep_going);
}

void Solver::reduceLoop(Puzzle *puzzle, bool *goodness)
{
    bool keep_going = true;
    while (keep_going)
    {
        // Run the solver stack and keep going if any of the methods yield a change
        keep_going = false;
        solveStack(puzzle, &keep_going);

        // Track the number of loop attempts we've made
        this->total_loops++;

        // Perform a validity check of the puzzle
        puzzle->validPuzzle(goodness);
        keep_going &= *goodness;
    }

    // Run one more strike for cleanup after the loop
    strike(puzzle, &keep_going);
}

void Solver::guessAndCheck()
{
    auto [bits, locs] = this->puzzle->leastPopularBit();

    // Loop over any bit not fully solved
    for (int i = 0; i < bits.size(); ++i)
    {
        uint16_t test_bit = bits[i];

        // Loop over every location where that bit is still possible
        for (const uint32_t &test_idx : locs[i])
        {
            // Make a copy of our puzzle and choose a value for one of the unsolved elements
            Puzzle test_puzzle = *this->puzzle;
            test_puzzle.setValue(test_idx, test_bit);
            test_puzzle.latest_solved_indices.push_back(test_idx);

            // Run through a reduce loop
            bool goodness = true;
            this->reduceLoop(&test_puzzle, &goodness);

            // If the loop yields an invalid puzzle, then we know our test index cannot be test bit
            if (!goodness)
            {
                // Remove test bit from the possible values at test index
                *this->puzzle->ptrValue(test_idx) -= *this->puzzle->ptrValue(test_idx) & test_bit;

                // Run another reduce loop on our true puzzle to squeeze out any benefits of this elimination
                this->reduceLoop(this->puzzle);
            }

            // Increment the number of guesses we've made
            this->guesses++;
        }
    }
}

void Solver::solve(int max_guesses)
{
    // Run the deterministic solve loop
    this->reduceLoop(this->puzzle);

    bool solvedFlag = this->puzzle->numUnsolved() == 0;
    while (!solvedFlag)
    {
        this->guessAndCheck();

        solvedFlag |= this->puzzle->numUnsolved() == 0;
        solvedFlag |= this->guesses >= max_guesses;
    }
}