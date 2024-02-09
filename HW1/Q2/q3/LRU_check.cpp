#define _GNU_SOURCE
#include <iostream>
#include <cstdint>
#include<random>
#include <x86intrin.h>
#include <windows.h>

void accessArray(volatile uint8_t* arr, int size) {
  // Access each address in the array
  for (int i = 0; i < size; ++i) {
    volatile uint8_t* addr = (arr + i);  // This is the memory address of arr[i]
  }
}

uint64_t time_access(volatile uint8_t* arr, int size) {
  uint64_t time1, time2;

  // Serializing instruction before timing
  __asm__ volatile (
      "CPUID\n\t"
      "RDTSC\n\t"
      : "=a"(time1)
      : : "%rbx", "%rcx", "%rdx"
  );

  // Access the addresses
  accessArray(arr, size);

  // Serializing instruction after timing
  __asm__ volatile (
      "RDTSCP\n\t"
      : "=a"(time2)
      : : "%rbx", "%rcx", "%rdx"
  );

  return time2 - time1;
}

uint64_t time_access_single_addr(volatile uint8_t *addr) {
  uint64_t time1, time2;
  volatile uint32_t tmp;
  // Serializing instruction before timing
  __asm__ volatile (
      "CPUID\n\t"
      "RDTSC\n\t"
      : "=a"(time1)
      : : "%rbx", "%rcx", "%rdx"
  );

  // Access the addresses
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

    //Using MB to denote size
    const int SIZE_L1 = 512 * 1024; //Size in KB
    const int SIZE_L2 = 4 * 1024 * 1024; //Size in MB
    const int SIZE_Cache = 16 * 1024 * 1024; //Size in MB
    const int SIZE_L3 = 12 * 1024 * 1024; // 12 MB

    volatile uint8_t *test = (volatile uint8_t *)malloc(SIZE_L3 * sizeof(uint8_t));

    //Allocating memory for the arrays
    volatile uint8_t *arr_L1 = (volatile uint8_t *)malloc(SIZE_L1 * sizeof(uint8_t));
    volatile uint8_t *arr_L2 = (volatile uint8_t *)malloc(SIZE_L2 * sizeof(uint8_t));
    volatile uint8_t *arr_Cache = (volatile uint8_t *)malloc(SIZE_Cache * sizeof(uint8_t));

    //checking if memory allocation is successful
    if(arr_L1==NULL || arr_L2==NULL || arr_Cache==NULL)
    {
        printf("Memory could not be allocated\n");
        return -1;
    }

    for(int j = 0; j < SIZE_L3;j++){
        test[j] = j;
    }
    // Filling caches
    for(int i = 0; i < SIZE_Cache; i++) {
        if(i < SIZE_L1) 
            arr_L1[i % SIZE_L1] = i; // Filling L1 cache
        if(i < SIZE_L2) 
            arr_L2[i % SIZE_L2] = i; // Filling L2 cache
        arr_Cache[i] = i; // Filling main cache
    }

    

    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the range for the random number

    __asm__ volatile (
        "CLFLUSH (%0)\n\t"
        :
        : "r"(arr_L1)
        : "memory"
    );  

    uint64_t time_L1 = time_access(arr_L1,SIZE_L1);

    __asm__ volatile (
        "CLFLUSH (%0)\n\t"
        :
        : "r"(arr_L2)
        : "memory"
    );  

    uint64_t time_L2 = time_access(arr_L2,SIZE_L2);

    __asm__ volatile (
        "CLFLUSH (%0)\n\t"
        :
        : "r"(test)
        : "memory"
    );  

    uint64_t test_time = time_access(test,SIZE_L3);

    std::cout << "Memory access time for L3 cache 12MB test in CPU cycles (Possible Cache Miss): " << test_time <<std::endl;

    // Flush addr from cache
    __asm__ volatile (
        "CLFLUSH (%0)\n\t"
        :
        : "r"(arr_Cache)
        : "memory"
    );  

    // Measure access time (possible cache miss - not in cache yet)
    uint64_t time_L3 = time_access(arr_Cache,SIZE_Cache);    

    std::cout << "Memory access time for L3 cache in CPU cycles (Possible Cache Miss): " << time_L3 <<std::endl;
    std::cout << "Memory access time for L2 cache in CPU cycles (Possible Cache Miss): " << time_L2 <<std::endl;
    std::cout << "Memory access time for L1 cache in CPU cycles (Possible Cache Miss): " << time_L1 <<std::endl;

// Generate a random number between 1 and 255

    std::uniform_int_distribution<int> distribution1(0, 512*1024-1);
    int random_number_L1 = distribution1(gen);
    volatile uint8_t *addr_L1 = &arr_L1[random_number_L1];

    uint64_t cycle_L1_cache_hit = time_access_single_addr(addr_L1);

    std::uniform_int_distribution<int> distribution2(0, 4 * 1024*1024-1);

    // Generate a random number between 1 and 255
    int random_number_L2 = distribution2(gen);
    volatile uint8_t *addr_L2 = &arr_L2[random_number_L2];

    uint64_t cycle_L2_cache_hit = time_access_single_addr(addr_L2);


    std::uniform_int_distribution<int> distribution3(0, 16 * 1024*1024-1);

    // Generate a random number between 1 and 255
    int random_number_L3 = distribution3(gen);
    volatile uint8_t *addr_L3 = &arr_Cache[random_number_L3];

    uint64_t cycle_L3_cache_hit = time_access_single_addr(addr_L3);

    // Measure access time again immediately (likely cache hit - should still be in cache)

    std::cout << "L1 Cache random access time (possible cache hit for L1 cache) : " << cycle_L1_cache_hit << " For random L1 idx "<< random_number_L1 << std::endl;
    std::cout << "L2 Cache random access time (possible cache hit for L2 cache) : " << cycle_L2_cache_hit << " For Random L2 idx " << random_number_L2 <<std::endl;
    std::cout << "L3 Cache random access time (possible cache hit for L3 cache) : " << cycle_L3_cache_hit << " For Random L3 idx " << random_number_L3 <<std::endl;



    // Allocating memory for new L3 array
    volatile uint8_t *arr_L3 = (volatile uint8_t *)malloc(SIZE_L3 * sizeof(uint8_t));

    // Checking if memory allocation is successful
    if (arr_L3==NULL) 
    {
        printf("Memory could not be allocated for L3 cache\n");
        return -1;
    }

    // Filling L3 cache
    for (int i = 0; i < SIZE_L3; i++) 
    {
        arr_L3[i] = i;
    }
    
    // Measure L3 Cache access time 
    uint64_t time_new_cache_fill = time_access(arr_L3, SIZE_L3);
    
    printf("New Cache fill access time for LLC (Possible cache miss) time: %llu\n", time_new_cache_fill);

    // Re-accessing the previously accessed random 4MB from main cache  
    uint64_t re_access_time_Cache = time_access_single_addr(addr_L3);

    std::cout << "Re-access time of previously accessed addr of random data in LLC Cache: "<< re_access_time_Cache << " For random L3 idx " << random_number_L3 <<std::endl;

    return 0;
}
