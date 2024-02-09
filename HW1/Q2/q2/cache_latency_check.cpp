#define _GNU_SOURCE
#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <cstdint>
#include <x86intrin.h>
#include <windows.h>

const int L1_cache_test = 8 * 1024;
const int L2_cache_test = 256 * 1024;
const int L3_cache_test =  8 * 1024 * 1024;

void accessData_2kb(int* array) {
    for(int i =0;i<2048;i++){
        volatile int temp = array[i];
    }
}

void accessData(int* array, int size) {
    for(int i =0;i<size;i++){
        volatile int temp = array[i];
    }
}

int main(){

    FILE *fptr;
    fptr = fopen("mem_access_time.csv","w");
    HANDLE hThread = GetCurrentThread();
    DWORD_PTR mask = 1; // Set affinity to CPU 0

    if (SetThreadAffinityMask(hThread, mask) == 0) {
        // Handle error
        return 1;
    } 

    int warmup_test = 2 * 1024;

    int* warmup_arr = (int*) malloc (warmup_test * sizeof(int));
    accessData(warmup_arr, warmup_test);
    accessData(warmup_arr, warmup_test);

    __asm__ volatile (
        "CLFLUSH (%0)\n\t"
        
        :
        : "r"(warmup_arr)
        : "memory"
    ); 
    
    std::vector<int> sizes = {static_cast<int>(L1_cache_test/sizeof(int)),
                              static_cast<int>(L2_cache_test/sizeof(int)),
                              static_cast<int>(L3_cache_test/sizeof(int))};  
    
    for(int size : sizes){


        uint64_t time1_miss, time2_miss;

        int* cache_arr = (int*) malloc(size * sizeof(int));


        //auto start = std::chrono::high_resolution_clock::now();
        __asm__ volatile ("mfence" ::: "memory");
        __asm__ volatile ("lfence" ::: "memory");
        __asm__ volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            : "=a"(time1_miss)
            : : "%rbx", "%rcx", "%rdx"
        );
        __asm__ volatile ("lfence" ::: "memory");

        accessData_2kb(cache_arr);

        __asm__ volatile ("lfence" ::: "memory");
        __asm__ volatile (
            "RDTSCP\n\t"
            : "=a"(time2_miss)
            : : "%rbx", "%rcx", "%rdx"
        );        
        __asm__ volatile ("lfence" ::: "memory");
        __asm__ volatile ("mfence" ::: "memory");

        accessData(cache_arr,size);

        auto duration_miss = (time2_miss - time1_miss);
        auto cache_latency = (duration_miss)/1024;
        std::cout << "Size: " << ((size * sizeof(int)) / (1024)) << "KB " 
            << ", Cache_latency is " << cache_latency << std::endl;
        free (cache_arr);
    }

    return 0;
}