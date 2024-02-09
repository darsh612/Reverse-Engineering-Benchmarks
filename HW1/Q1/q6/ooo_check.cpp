#include <iostream>
#include <chrono>

#define ITERATIONS 1000000
#define DELAY 1000

inline void dependent_instructions() {
    volatile int a = 1;
    for (int i = 0; i < DELAY; ++i) { // simulate computational complexity
        a += a * i; // heavily dependent computations
    }
}

inline void independent_instructions() {
    volatile int arr[DELAY]; 
    for (int i = 0; i < DELAY; ++i) { // array induced delay
        arr[i] = i;
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; i++) {
        dependent_instructions();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_dep = end-start;

    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; i++) {
        independent_instructions();
    }

    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_ind = end-start;

    std::cout << "Dependent instructions execution time: " << diff_dep.count() << " s\n";
    std::cout << "Independent instructions execution time: " << diff_ind.count() << " s\n";

    if (diff_dep.count() > diff_ind.count()) {
        std::cout << "The processor seems to be able to execute instructions out of order!\n";
    } else {
        std::cout << "The processor might not perform out-of-order execution or the effect isn't observable due to other factors!\n";
    }

    return 0;
}