#include <stdio.h>
#include <stdint.h>

#define NUM_ITERATIONS 1000000

void dependentInstructions() {
    uint64_t sum = 0;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sum += i;
    }
}

void independentInstructions() {
    uint64_t sum1 = 0;
    uint64_t sum2 = 0;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sum1 += i;
        sum2 += i;
    }
}

int main() {
    printf("Running Dependent Instructions:\n");
    unsigned long long start_time = __builtin_ia32_rdtsc();
    dependentInstructions();
    unsigned long long end_time = __builtin_ia32_rdtsc();
    printf("Cycles taken: %llu\n", end_time - start_time);

    printf("Running Independent Instructions:\n");
    start_time = __builtin_ia32_rdtsc();
    independentInstructions();
    end_time = __builtin_ia32_rdtsc();
    printf("Cycles taken: %llu\n", end_time - start_time);

    return 0;
}

/*A microbenchmark to infer if your processor is pipelined or not can be designed to measure the performance difference between dependent and independent instructions. Pipelining is a common technique in modern processors that allows multiple instructions to be executed in overlapping stages. If your processor is pipelined, you should observe a significant performance improvement when instructions are independent compared to when they have data dependencies.

In this microbenchmark, we have two functions, dependentInstructions and independentInstructions. The dependentInstructions function performs a loop with dependent instructions, where each iteration relies on the result of the previous one. The independentInstructions function, on the other hand, performs a similar loop, but the instructions are independent, allowing the processor to potentially execute them in parallel.

The benchmark measures the number of cycles taken to execute each set of instructions using the rdtsc instruction, which reads the timestamp counter. If your processor is pipelined, you should observe a significant difference in the number of cycles between the two runs. Pipelined processors can overlap the execution of independent instructions, leading to faster execution.

Keep in mind that the exact behavior may vary depending on the specific architecture and microarchitecture of your processor. Additionally, other factors such as cache effects can also impact the results.

If your processor is not pipelined or has a very simple and shallow pipeline, you may still observe a difference in timing between dependent and independent instructions, but the difference might not be as pronounced as it would be on a deeply pipelined processor.
*/
