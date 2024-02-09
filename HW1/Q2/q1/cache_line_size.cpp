#define _GNU_SOURCE
#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <cstdint>
#include <x86intrin.h>
#include <windows.h>

int main() {

    HANDLE hThread = GetCurrentThread();
    DWORD_PTR mask = 1; // Set affinity to CPU 0

    if (SetThreadAffinityMask(hThread, mask) == 0) {
        // Handle error
        return 1;
    }
    
    FILE *fptr;
    fptr = fopen("cache_access_times_optimized.csv","w");
    using namespace std::chrono;

    constexpr int arraySize = 2 * 1024 * 1024; // 2MB
    constexpr int strideMax = 512; // Maximum stride size




    for (int stride = 1; stride <= strideMax; stride++) {
        // Warm up cache
        
        int* array = (int*) malloc(2*1024*1024*sizeof(int));
        
        for (int i = 0; i < 2*1024*1024; i++){
            array[i] = 1;
        }

        for (int i = 0; i < arraySize; i += stride){
            volatile int temp = array[i];
        }

        __asm__ volatile (
            "CLFLUSH (%0)\n\t"

            :
            : "r"(array)
            : "memory"
        );     

        // Benchmark
        auto startTime = high_resolution_clock::now();

        for (int i = 0; i < arraySize; i += stride){
            array[i] *= 3;
            volatile int temp = array[i];
        }

       auto endTime = high_resolution_clock::now();
        __asm__ volatile (
            "CLFLUSH (%0)\n\t"

            :
            : "r"(array)
            : "memory"
        ); 


       std::cout << "Stride " << stride << ": " 
                 << duration_cast<nanoseconds>(endTime - startTime).count() 
                 << " nanoseconds" << std::endl;
        fprintf(fptr,"%d,%d\n",stride,duration_cast<nanoseconds>(endTime - startTime).count());

    }

    return 0;
}

/*The general concept would be to write a program that tests memory access times for different strides through an array of data. When the stride size equals the cache line size, you should see a notable increase in access time, because each memory access will have to fetch a new cache line.

Now this concept works because, cache controller generally brings contiguous memory block know as cache lines into the cache. Suppose we have a continuous memory addresses from A0-A1000 in main memory, let our cache line size be 16. So when it bring cacheline with memory address A0-A15 into the cache. Now depending on the striding, we can see if it is equal stride is equal to cache line size or not. Because stride of 1 will cause alot of cache hits, like after 1st miss all 15 will be cache hit, in stride of 2 that will be 1 miss and 7 cache hit, but in case stride of 16, there will be 1 cache miss, and then it will go to A16, which is not in cache line, so again a cache miss, and so on
*/