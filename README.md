# sudofun
A C++ repository for solving sudoku puzzles more efficiently than simple backtracking algorithms. The over-arching paradigm is to model the solution space for each puzzle element as a nine bit value, which allows us to leverage bitwise operations in many of the solving steps. This repository began as a Python project, and retains that source code under `sudofun_py`, but it is no longer under development.

The build supports both a command line interface as well as a basic GUI (using Qt5) for entering and solving puzzles. 


![](/misc/guiunsolved.png) ![](/misc//guisolved.png)