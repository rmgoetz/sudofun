
#include "solver.hpp"
#include <chrono>
#include <fstream>
#include <exception>

void runBenchmark(const std::string &filename, uint32_t maxGuesses, uint32_t loops, bool warmup)
{
    using clock = std::chrono::steady_clock;
    using duration = std::chrono::nanoseconds;

    if (warmup)
    {
        std::cout << "Running warmup" << std::endl;
    }
    else
    {
        std::cout << "Running test loop" << std::endl;
    }

    std::ifstream cluefile(filename);
    if (!cluefile.is_open())
    {
        std::cout << "Could not open clue file" << std::endl;
        return;
    }

    std::string clue_string;
    duration elapsed_time_ns = duration(duration::zero());
    uint64_t solve_count{0};
    uint64_t fail_count{0};
    for (uint32_t loop_idx = 0; loop_idx < loops; ++loop_idx)
    {
        cluefile.clear();
        cluefile.seekg(0, std::ios::beg);

        while (std::getline(cluefile, clue_string))
        {
            if (!(clue_string.length() == 81)){
                throw std::runtime_error("String is not proper length!!!");
            }

            Puzzle puzzle = Puzzle();
            puzzle.addBenchmarkString(clue_string);
            Solver solver = Solver(&puzzle);

            auto start = clock::now();
            solver.solve(maxGuesses);
            auto end = clock::now();

            elapsed_time_ns += std::chrono::duration_cast<duration>(end - start);

            if ((puzzle.numUnsolved() == 0) && (puzzle.validPuzzle()))
            {
                ++solve_count;
            }
            else
            {
                ++fail_count;
            }
        }
    }

    if (!warmup)
    {
        long long total_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time_ns).count();
        double puzzles_per_ms = (double)(solve_count + fail_count) / ((double)total_time_ms);
        std::cout << "Solved " << solve_count << " puzzles and failed to solve " << fail_count << " puzzles"
                  << "\nTook " << total_time_ms << " ms to solve"
                  << "\n" << puzzles_per_ms << " puzzles per millisecond"
                  << std::endl;
    }
}

int main(int argc, char *argv[])
{

    if (argc < 5)
    {
        std::cout << "Usage: " << argv[0] << " <filename> <max guesses> <warmup loops> <loops>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    uint32_t max_guesses = static_cast<uint32_t>(std::stol(argv[2]));
    uint32_t warmup_loops = static_cast<uint32_t>(std::stol(argv[3]));
    uint32_t loops = static_cast<uint32_t>(std::stol(argv[4]));

    std::cout << "Testing using file: " << filename
              << "\nmaxGuesses: " << max_guesses
              << "\nwarmupLoops: " << warmup_loops
              << "\ntestLoops: " << loops
              << std::endl;

    try
    {
        runBenchmark(filename, max_guesses, warmup_loops, true);
        runBenchmark(filename, max_guesses, loops, false);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught" << std::endl;
        return 1;
    }

    return 0;
}