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
    int min_iteration = 128;

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

int TLB_hit_size = 32 * 1024 * 1024;
    int* array = (int*) malloc(TLB_hit_size*sizeof(int));

    for (int i = 0; i < TLB_hit_size; i++){
        array[i] = 1;
    }

    int num_iterations = TLB_hit_size/page_size;

    if(num_iterations < min_iteration){
        num_iterations = min_iteration;
    }

    for (int i = 0; i < TLB_hit_size; i++){
        volatile int temp = array[i];
    }
    __asm__ volatile (
        "CLFLUSH (%0)\n\t"
        :
        : "r"(array)
        : "memory"
    );    

        // Benchmark
        uint64_t startTime_hit, endTime_hit;

        __asm__ volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            : "=a"(startTime_hit)
            : : "%rbx", "%rcx", "%rdx"
        );


        for (int i = 0; i < num_iterations; i++){
            size_t index = (i * page_size) % TLB_hit_size; 
            volatile int temp = array[index];
        }

        __asm__ volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            : "=a"(endTime_hit)
            : : "%rbx", "%rcx", "%rdx"
        );
       //auto endTime_hit = high_resolution_clock::now();
        __asm__ volatile (
            "CLFLUSH (%0)\n\t"

            :
            : "r"(array)
            : "memory"
        ); 

        uint64_t TLB_hit_time = (endTime_hit - startTime_hit)/(num_iterations);

       std::cout << " TLB Hit time is " << TLB_hit_time << std::endl;
        //fprintf(fptr,"%d\n",duration_cast<nanoseconds>(endTime - startTime).count());

    int TLB_miss_size = 256 * 1024 * 1024;
    int* array_miss = (int*) malloc(TLB_miss_size*sizeof(int));

    for (int i = 0; i < TLB_miss_size; i++){
        array_miss[i] = 1;
    }

    int num_iterations_1 = TLB_miss_size/page_size;

    if(num_iterations_1 < min_iteration){
        num_iterations_1 = min_iteration;
    }

    for (int i = 0; i < TLB_miss_size; i++){
        volatile int temp = array_miss[i];
    }
    __asm__ volatile (
        "CLFLUSH (%0)\n\t"
        :
        : "r"(array_miss)
        : "memory"
    );    

        // Benchmark
    uint64_t startTime_miss, endTime_miss;

    __asm__ volatile (
    "CPUID\n\t"
    "RDTSC\n\t"
    : "=a"(startTime_miss)
    : : "%rbx", "%rcx", "%rdx"
    );

        for (int i = 0; i < num_iterations_1; i++){
            size_t index = (i * page_size) % TLB_miss_size; 
            volatile int temp = array_miss[index];
        }

    __asm__ volatile (
    "CPUID\n\t"
    "RDTSC\n\t"
    : "=a"(endTime_miss)
    : : "%rbx", "%rcx", "%rdx"
    );

      // auto endTime_miss = high_resolution_clock::now();
        __asm__ volatile (
            "CLFLUSH (%0)\n\t"

            :
            : "r"(array_miss)
            : "memory"
        ); 

        uint64_t TLB_miss_time = (endTime_miss - startTime_miss)/(num_iterations);

       std::cout << " TLB miss time is "<< TLB_miss_time << std::endl;

       std::cout << " TLB miss latency is " << TLB_miss_time - TLB_hit_time << std::endl;  

}