#define _GNU_SOURCE
#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <cstdint>
#include <x86intrin.h>
#include <windows.h>
#include<unordered_set>

const int rand_size1 = 4 * 1024;
const int rand_size2 = 8 * 1024;
const int rand_size3 = 16 * 1024;
const int sizeL1 = 32 * 1024;
const int rand_size4 = 64 * 1024;
const int rand_size5 = 128 * 1024;
const int rand_size6 = 256 * 1024;
const int sizeL2 = 512 * 1024;
const int rand_size7 = 1 * 1024 * 1024;
const int rand_size8 = 2 * 1024 * 1024;
const int rand_size9 = 4 * 1024 * 1024;
const int rand_size10 = 8 * 1024 * 1024;
const int sizeL3 = 16 * 1024 * 1024;
const int rand_size11 = 32 * 1024 * 1024;
const int rand_size12 = 64 * 1024 * 1024;
const int startValue = 100;
const unsigned int SEED = 42; 



// Pseudo-random Memory Access within the given size range
void accessData(int* array, int size) {
    for(int i =0;i<size;i++){
        volatile int temp = array[i];
    }
}

void accessData_1kb(int* array, int size) {
    // Create a random device and generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(size/2, size-1);
    for(int i = 0;i<1*1024;i++){
        volatile int temp = array[distr(gen)];
    }
}

int main() {

    FILE *fptr;
    fptr = fopen("mem_access_time.csv","w");
    HANDLE hThread = GetCurrentThread();
    DWORD_PTR mask = 1; // Set affinity to CPU 0

    if (SetThreadAffinityMask(hThread, mask) == 0) {
        // Handle error
        return 1;
    }
    std::vector<int> sizes = {static_cast<int>(rand_size1/sizeof(int)),
                              static_cast<int>(rand_size2/sizeof(int)),
                              static_cast<int>(rand_size3/sizeof(int)),
                              static_cast<int>(sizeL1/sizeof(int)),
                              static_cast<int>(rand_size4/sizeof(int)), 
                              static_cast<int>(rand_size5/sizeof(int)),
                              static_cast<int>(rand_size6/sizeof(int)),
                              static_cast<int>(sizeL2/sizeof(int)),
                              static_cast<int>(rand_size7/sizeof(int)),
                              static_cast<int>(rand_size8/sizeof(int)), 
                              static_cast<int>(rand_size9/sizeof(int)),
                              static_cast<int>(rand_size10/sizeof(int)),
                              static_cast<int>(sizeL3/sizeof(int)), 
                              static_cast<int>(rand_size11/sizeof(int)),
                              static_cast<int>(rand_size12/sizeof(int))};

    int* eg_array = (int*) malloc( 2 * 1024 * sizeof(int));
    accessData(eg_array, 2*1024);
    accessData_1kb(eg_array, 2*1024);

    __asm__ volatile (
        "CLFLUSH (%0)\n\t"
        
        :
        : "r"(eg_array)
        : "memory"
    ); 

    for (int size : sizes) {
         int* array = (int*) malloc(size * sizeof(int));
        //std::fill_n(array, size, startValue);

        __asm__ volatile (
            "CLFLUSH (%0)\n\t"
            
            :
            : "r"(array)
            : "memory"
        ); 

        uint64_t time1, time2;

        accessData(array, size);

        auto start = std::chrono::high_resolution_clock::now();
        __asm__ volatile ("mfence" ::: "memory");
        __asm__ volatile ("lfence" ::: "memory");
        __asm__ volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            : "=a"(time1)
            : : "%rbx", "%rcx", "%rdx"
        );
        __asm__ volatile ("lfence" ::: "memory");
        accessData_1kb(array,size);
        //auto stop = std::chrono::high_resolution_clock::now();
        __asm__ volatile ("lfence" ::: "memory");
        __asm__ volatile (
            "RDTSCP\n\t"
            : "=a"(time2)
            : : "%rbx", "%rcx", "%rdx"
        );        
        __asm__ volatile ("lfence" ::: "memory");
        __asm__ volatile ("mfence" ::: "memory");
        __asm__ volatile (
            "CLFLUSH (%0)\n\t"
            
            :
            : "r"(array)
            : "memory"
        ); 
        //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        auto duration = (time2 - time1)/(1024);
        std::cout << "Size: " << ((size * sizeof(int)) / (1024)) << "KB"
            << ", Time: " << duration << std::endl;
        fprintf(fptr,"%d KB,%llu\n", ((size * sizeof(int)) / (1024)), duration);
        free (array);
    }

    return 0;
}