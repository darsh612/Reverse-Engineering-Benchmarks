#include <iostream>
#include <chrono>

const long int iterations = 1000000;
const int iter = 20;

int main() {

    FILE *fptr;
    fptr = fopen("mem_access_time.csv","w");
    // Initialize a variable to accumulate a result
    for(int stride =1;stride<4096;stride= stride*2){

        long int result = 0;

        // Start the timer
        auto start = std::chrono::high_resolution_clock::now();

        // Loop without conditional branches
        for (long int i = 0; i < iterations; i = i+stride) {
            result += 1;
        }

        // Stop the timer
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the elapsed time for the loop without branches
        double elapsed_secs_no_branch = std::chrono::duration<double>(end - start).count() * 1000000.0;

        // Reset the result
        result = 0;

        // Start the timer again
        start = std::chrono::high_resolution_clock::now();

        // Loop with conditional branches
        for (long int i = 0; i < iterations; i = i+stride) {
            if (i % 2 == 0) {
                result += 1;
            } else {
                result -= 1;
            }
        }

        // Stop the timer
        end = std::chrono::high_resolution_clock::now();

        // Calculate the elapsed time for the loop with branches
        double elapsed_secs_branch = std::chrono::duration<double>(end - start).count() * 1000000.0;

        // Print the results
        std::cout << "Execution Time Without Branches with stride " << stride <<" : " << elapsed_secs_no_branch << " microseconds\n";
        std::cout << "Execution Time With Branches with stride "<< stride << " : " << elapsed_secs_branch << " microseconds\n";
        fprintf(fptr,"elapsed with no branch, elapsed with branch");
        fprintf(fptr,"%lld miliseconds,%lld miliseconds\n", (elapsed_secs_no_branch, elapsed_secs_branch));
    }

    return 0;
}

/*/*In this code, we have two loops: one without conditional branches and one with conditional branches. We measure and compare the execution time of both loops.

Here's how to interpret the results:

If the execution time of the loop without branches is significantly shorter than the loop with branches, it suggests that your processor is handling the branches efficiently, likely due to an effective branch predictor.

If the execution times of both loops are similar, it may indicate that the branch predictor on your specific processor is less effective, and it's having a similar impact on both code paths.

Keep in mind that this approach is still not a definitive test of your processor's branch predictor. It provides a relative comparison of execution times with and without branching behavior. A shorter execution time for the loop without branches compared to the loop with branches suggests better branch prediction performance.*/