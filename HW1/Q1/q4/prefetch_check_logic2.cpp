#include <iostream>
#include <chrono>
#include <vector>

#define MAX_STRIDE 10
#define TOTAL_ACCESSES 16000000  // Keep total memory accesses constant
#define REPETITIONS 10

struct Node {
    int data;
    Node* next;
};

int main() {
    std::vector<int> data(TOTAL_ACCESSES * MAX_STRIDE, 1);  // Adjust the data size
    volatile int sink = 0;
    for (int stride = 1; stride <= MAX_STRIDE; stride++) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int repeat = 0; repeat < REPETITIONS; repeat++) {
            for (int i = 0; i < TOTAL_ACCESSES; i++) {  // Maintain the total number of accesses
               sink = data[i * stride];  // Access at intervals of the stride
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Time taken by array with stride " << stride << ": " 
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                  << "ms" << std::endl;
    }
    Node* head = nullptr;
    Node* curr = nullptr;
    
    // Create linked list with TOTAL_ACCESSES nodes
    for (int i = 0; i < TOTAL_ACCESSES; i++) {
        Node* newNode = new Node();
        newNode->data = 1;  // Set data
        
        if (head == nullptr) {
            head = newNode;
        }
        else {
            curr->next = newNode;
        }
        
        curr = newNode;
    }
    curr->next = head; // Form a circular linked list

    for (int stride = 1; stride <= MAX_STRIDE; stride++) {
        auto start1 = std::chrono::high_resolution_clock::now();
        for (int repeat = 0; repeat < REPETITIONS; repeat++) {
            Node* ptr = head;
            for (int i = 0; i < TOTAL_ACCESSES; i++) {
                int value = ptr->data;  // Access data through pointer
                for (int j = 0; j < stride; j++) {
                    ptr = ptr->next;  // Move to next node
                }
            }
        }
        auto end1 = std::chrono::high_resolution_clock::now();
        std::cout << "Time taken by circular linked list with stride " << stride << ": " 
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count()
                  << "ms" << std::endl;
    }


    // Clean up memory
    curr = head;
    Node* next_node;
    do {
        next_node = curr->next;
        delete curr;
        curr = next_node;
    } while (curr != head);    

        return 0;
}


/*
Prefetching is a hardware technique where upcoming data that might be needed soon is brought into the cache ahead of time with the goal of reducing memory latency.

In this microbenchmark, the prefetcher's existence and effectiveness can be inferred by analyzing the stride effect on the memory accesses. Here's how:

- With a stride of one (sequential accesses), the prefetcher can very easily predict the data we're going to need next (because we're just going in order). Therefore, it can fetch that data from memory into the cache before we need it. The consequent benefit is higher than any other stride. This is why the access time for stride-one is usually the lowest.

- For larger strides, the prefetcher faces difficulty predicting the next memory access because we're jumping around memory. Some prefetchers can handle certain strides but as the stride length increases, the effectiveness of prefetching decreases.

So, if the access time increases as the stride increases (while the total number of memory accesses stays the same), then it's a strong indicator that prefetching is working. Specifically, if you notice that the time for stride-one access is significantly lower than for larger strides, it means your prefetcher is doing a good job optimizing your memory accesses.

But with increase in the strides, won't the timing be short as there will be few elements to access ?

ou're correct. With larger strides, you're skipping more elements, resulting in fewer overall memory accesses. This fact is important and might affect the timing measurements such that you see less time with larger strides, but that wouldn't necessarily mean the prefetcher isn't working.

In the context of our benchmark, therefore, we're not simply increasing the stride—we're also adjusting the access pattern accordingly to ensure that the total number of memory accesses remains constant, regardless of the stride. This is achieved by multiplying the current index `i` in the loop with the `stride`, which essentially spreads out the memory accesses while keeping the total number constant:

```cpp
for (int i = 0; i < TOTAL_ACCESSES; i++) {
    sink = data[i * stride];
}
```
Here, the total number of loop iterations—and thereby memory accesses—is always `TOTAL_ACCESSES`, regardless of the stride value. The `i * stride` computation determines where in memory we're accessing data for each iteration.

With a stride of 1, we're accessing every element. With a stride of 2, we're accessing every second element, and so on. This achieves a constant amount of memory accesses with varying access patterns, allowing us to test the effectiveness of the prefetcher under different conditions. 

Note that prefetching is really designed for sequential or nearly sequential access. Even a small constant stride can disrupt its functionality, not to mention larger strides. With larger strides, you're essentially moving further away from the access pattern that prefetching is optimized for, which is why the performance decreases.
*/