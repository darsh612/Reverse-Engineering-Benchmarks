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
    DWORD_PTR mask = 2; // Set affinity to CPU 0

    int page_size = 4 * 1024;
    int min_iteration = 8192;

    if (SetThreadAffinityMask(hThread, mask) == 0) {
        // Handle error
        return 1;
    }
    
    FILE *fptr;
    fptr = fopen("TLB_access_times_optimized.csv","w");
    using namespace std::chrono;

    constexpr int strideMax = 32 * 1024 * 1024; // Maximum stride size

    int eg_arraySize = 128 * 1024 *1024;
    int *eg_array = (int*) malloc(eg_arraySize*sizeof(int));

    for (int i = 0; i < eg_arraySize; i++){
        eg_array[i] = 1;
    }

    for (int i = 0; i < eg_arraySize; i++){
        volatile int temp1 = eg_array[i];
    }    

    __asm__ volatile (
        "CLFLUSH (%0)\n\t"
        :
        : "r"(eg_array)
        : "memory"
    );     

    free(eg_array);

    int x = 1;

    for (int stride = page_size; stride <= strideMax; stride = x * page_size) {
        // Warm up cache
        int arraySize = 1024 * 1024 * 1024;
        int* array = (int*) malloc(arraySize*sizeof(int));
        
        for (int i = 0; i < arraySize; i++){
            array[i] = 1;
        }

        for (int i = 0; i < arraySize; i += stride){
            volatile int temp = array[i];
        }

        int num_iterations = arraySize/stride;
        __asm__ volatile (
            "CLFLUSH (%0)\n\t"

            :
            : "r"(array)
            : "memory"
        );     

        // Benchmark
        auto startTime = high_resolution_clock::now();

        if(num_iterations < min_iteration){
            num_iterations = min_iteration;
        }

        for (int i = 0; i < num_iterations; i++){
            size_t index = (i * stride) % arraySize; 
            array[i] *= 3;
            volatile int temp = array[index];
        }

       auto endTime = high_resolution_clock::now();
        __asm__ volatile (
            "CLFLUSH (%0)\n\t"

            :
            : "r"(array)
            : "memory"
        ); 


       std::cout << "Entries : " << x << " Stride " << stride/1024 << " KB : " 
                 << duration_cast<nanoseconds>(endTime - startTime).count()/(1000000.0) 
                 << " miliseconds" << std::endl;
        fprintf(fptr," %d,%d,%d\n",x,stride/1024,duration_cast<nanoseconds>(endTime - startTime).count()/(1000000.0));
        x = x * 2;
        free(array);
    }

    return 0;
}