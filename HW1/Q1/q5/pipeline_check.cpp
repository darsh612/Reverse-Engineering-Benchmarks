#include <chrono>
#include <iostream>

#define SIZE 100000000

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    // The result of each operation is dependent on the previous one
    volatile int a = 1;
    for (int i = 0; i < SIZE; ++i) {
        a *= 2;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time taken by dependent instructions: " << diff.count() << "s\n";

    start = std::chrono::high_resolution_clock::now();
    // The result of each operation is independent
    volatile int e = 1, f = 2;
    for (int i = 0; i < SIZE; ++i) {
        e *= 2;
        f *= 2;
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;

    std::cout << "Time taken by 2 independent instructions: " << diff.count() << "s\n";

    start = std::chrono::high_resolution_clock::now();
    // The result of each operation is independent
    volatile int b = 1, c = 2, d = 3;
    for (int i = 0; i < SIZE; ++i) {
        b *= 2;
        c *= 2;
        d *= 2;
    }
    end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    std::cout << "Time taken by 3 independent instructions: " << diff.count() << "s\n";

    return 0;
}