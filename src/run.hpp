
#ifndef SUDOFUN_RUN_SOLVERS_HEADER
#define SUDOFUN_RUN_SOLVERS_HEADER

void runConsoleSolve(int maxGuesses);

#if defined(BUILT_WITH_QT5)

int runGuiSolve(int maxGuesses, int argc, char *argv[]);

#endif
#endif