# sudofun
A C++ repository for solving sudoku puzzles more efficiently than simple backtracking algorithms. The over-arching paradigm is to model the solution space for each puzzle element as a nine bit value, which allows us to leverage bitwise operations in many of the solving steps. The solver uses methods based on human heuristics for solving sudoku. (For a more formal -- and more optimized -- approach, see this from [this from Tom Dillon](https://t-dillon.github.io/tdoku/)) This repository began as a Python project, and retains that source code under `sudofun_py`, but the Python component is no longer under development.

The build supports both a command line interface as well as a basic GUI (using Qt5) for entering and solving puzzles. The GUI is straightforward and illustrated below:

![](/misc/guiunsolved.png) ![](/misc//guisolved.png)

For the command line interface, note that the input format for clue strings is not the conventional 81 character long string but is instead a series of colon delimited triplets, where the first value is the row index (indexed from 1), second value is column index (indexed from 1), and third value is cell value. As an example, the clue string for the puzzle in the above GUI demonstration could be:
```
132:145:178:194:231:314:348:421:469:473:488:523:536:587:634:653:766:843:854:891:916:927:999
``` 
This unusual convention was chosen for debugging purposes as it has the benefits of being easy to modify, independent of order, and allows us to overwrite clues earlier in the string by simply appending clues to the end of the string.

## Benchmarks

To benchmark performance we test against the datasets provided at [this very helpful repository](https://github.com/grantm/sudoku-exchange-puzzle-bank). For each dataset, we run the solver in both a purely heuristic (no guessing) mode, as well as a mode which allows for a guess-and-check approach when the hueristics alone fail to solve the puzzle. While this guess-and-check mode guarantees that all of the dataset puzzles will be solved, it is a significant performance hit. The benchmarks in the table below were taken on a machine with an AMD Ryzen 7 3700X.

| Dataset | Size (puzzles) | 0-Guess (puzzle/s) | 0-Guess Solved | w/Guess (puzzle/s) | w/Guess Solved |
| --- | --- | --- | --- | --- | --- |
| Easy | 100 000 | 36 400 | 100 000 (100%) | N/A | N/A |
| Medium | 352 643 | 22 800 | 335 049 (95%) | 9 780 | 352 643 (100%) |
| Hard | 321 592 | 13 300 | 63 937 (20%) | 985 | 321 592 (100%) |
| Diabolical | 119 681 | 13 100 | 0 (0%) | 567 | 119 681 (100%) |