// high_resolution_clock example
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <string>

int main()
{
    using namespace std::chrono;

    high_resolution_clock::time_point now = high_resolution_clock::now();

    int mod = 9;
    std::cout << "Choose your modulus (integer): "; // comment out when not standalone
    std::cin >> mod; // comment out when not standalone

    auto val = now.time_since_epoch();
    unsigned long t = val.count();

    unsigned long a, b, c, d, e, f, g;

    a = (t / 100) % 10000;
    b = a ^ 10922;
    c = a ^ 5461;
    d = a ^ 3276;
    e = a ^ 13107;
    f = (b << (e % 3)) * (c << (b % 5)) * (d << (c % 7)) * (e << (d % 11));
    std::string w = std::to_string(f);
    w.resize(4);
    g = std::stoul(w);

    std::cout << (g % mod) + 1; // comment out when not standalone

    return 0 //(g % mod) + 1;
}