
#ifndef SUDOFUN_SOLVER_CLASS_HEADER
#define SUDOFUN_SOLVER_CLASS_HEADER

// Forward declare puzzle
class Puzzle;

class Solver
{
private:
    Puzzle *puzzle;
    int total_loops;
    int guesses;

public:
    Solver(Puzzle *puzzle) : puzzle(puzzle), total_loops(0), guesses(0) {}

    // Solver rules
    void strike(Puzzle *puzzle, bool *updated);
    void unique(Puzzle *puzzle, bool *updated);
    void pigeon(Puzzle *puzzle, bool *updated);
    void squeeze(Puzzle *puzzle, bool *updated);
    void pipe(Puzzle *puzzle, bool *updated);
    void solveStack(Puzzle *puzzle, bool *updated);
    void postStepUpdate(Puzzle *puzzle);

    // An iterative deterministic solver loop
    void reduceLoop(Puzzle *puzzle);
    void reduceLoop(Puzzle *puzzle, bool *goodness);

    // Guesser solver
    void guessAndCheck();

    //
    void solve(int max_guesses = 100);
};

#endif