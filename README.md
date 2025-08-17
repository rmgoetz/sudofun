# sudofun
A C++ repository for solving sudoku puzzles more efficiently than simple backtracking algorithms. The over-arching paradigm is to model the solution space for each puzzle element as a nine bit value, which allows us to leverage bitwise operations in many of the solving steps. This repository began as a Python project, and retains that source code under `sudofun_py`, but the Python component is no longer under development.

The build supports both a command line interface as well as a basic GUI (using Qt5) for entering and solving puzzles. The GUI is straightforward and illustrated below:

![](/misc/guiunsolved.png) ![](/misc//guisolved.png)

For the command line interface, note that the input format for clue strings is not the conventional 81 character long string but is instead a series of colon delimited triplets, where the first value is the row index (indexed from 1), second value is column index (indexed from 1), and third value is cell value. As an example, the clue string for the puzzle in the above GUI demonstration could be:
```
132:145:178:194:231:314:348:421:469:473:488:523:536:587:634:653:766:843:854:891:916:927:999
``` 