#define _GNU_SOURCE
#include <iostream>
#include <cstdint>
#include<random>
#include <x86intrin.h>
#include <windows.h>

uint64_t time_access(volatile uint8_t *addr) {
    uint64_t time1, time2;
    volatile uint32_t tmp;

    // Serializing instruction before timing
    __asm__ volatile (
        "CPUID\n\t"
        "RDTSC\n\t"
        : "=a"(time1)
        : : "%rbx", "%rcx", "%rdx"
    );

    // Access the value
    tmp = *addr;

    // Serializing instruction after timing
    __asm__ volatile (
        "RDTSCP\n\t"
        : "=a"(time2)
        : : "%rbx", "%rcx", "%rdx"
    );

    return time2 - time1;
}

int main() {

    SetThreadAffinityMask(GetCurrentThread(), 1);

    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the range for the random number
    std::uniform_int_distribution<int> distribution(0, 1024*1024-1);

    // Generate a random number between 1 and 255
    int random_number = distribution(gen);

    uint8_t array[1024*1024];
    uint8_t *addr = &array[random_number];

    // Fill array with some data
    /*for (int i = 0; i < 256; ++i) {
        array[i] = i;
    }*/

    // Flush addr from cache
    __asm__ volatile (
        "CLFLUSH (%0)\n\t"
        :
        : "r"(addr)
        : "memory"
    );  

    // Measure access time (possible cache miss - not in cache yet)
    uint64_t cyclesCacheMiss = time_access(addr);

    // Measure access time again immediately (likely cache hit - should still be in cache)
    uint64_t cyclesCacheHit = time_access(addr);

    std::cout << "First access time in CPU cycles (Possible Cache Miss): " << cyclesCacheMiss << " For randomized index : " << random_number <<std::endl;
    std::cout << "Second access time in CPU cycles (Possible Cache Hit): " << cyclesCacheHit <<  " For randomized index : " << random_number << std::endl;
    return 0;
}