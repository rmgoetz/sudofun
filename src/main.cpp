
#include "run.hpp"
#include <string_view>

int main(int argc, char *argv[])
{

    bool runConsole{false};
    int maxGuesses{1};
    bool nineBit{false};

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-C" || arg == "-c" || arg == "--console")
        {
            runConsole = true;
        }
        else if (arg == "--max-guesses" && i + 1 < argc)
        {
            maxGuesses = std::stoi(argv[++i]); // parse next argument as int
        }
        else if (arg == "--nine-bit")
        {
            nineBit = true;
        }
        else
        {
            std::cerr << "Unknown or incomplete argument: " << arg << "\n";
            return 1;
        }
    }

    // Parse input

#ifndef BUILT_WITH_QT5

    runConsoleSolve(maxGuesses, nineBit);

#else

    if (runConsole)
    {
        runConsoleSolve(maxGuesses, nineBit);
    }

    else
    {
        runGuiSolve(maxGuesses, argc, argv);
    }

    return 0;

#endif
}
