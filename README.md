# sudofun
A repository for solving sudoku puzzles more efficiently than simple backtracking algorithms.

The over-arching paradigm is to model the solution space for each puzzle element as a nine bit value, which allows us to leverage bitwise operations in many of the solving steps.