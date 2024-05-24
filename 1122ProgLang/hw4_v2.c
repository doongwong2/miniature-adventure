#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 4
#define MAX_NUMBER 100

int sum = 0;
pthread_mutex_t sum_lock;

// Function to compute the sum of numbers from start to end
void* compute_sum(void* arg) {
    int thread_id = *(int*)arg;
    int start = thread_id * (MAX_NUMBER / NUM_THREADS) + 1;
    int end = (thread_id + 1) * (MAX_NUMBER / NUM_THREADS);
    if (thread_id == NUM_THREADS - 1) {
        end = MAX_NUMBER;
    }
    
    int partial_sum = 0;
    for (int i = start; i <= end; ++i) {
        partial_sum += i;
    }
    
    // Lock the mutex before updating the shared sum variable
    pthread_mutex_lock(&sum_lock);
    sum += partial_sum;
    pthread_mutex_unlock(&sum_lock);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&sum_lock, NULL);

    // Create threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        int* thread_id = malloc(sizeof(int));
        *thread_id = i;
        pthread_create(&threads[i], NULL, compute_sum, (void*)thread_id);
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Print the total sum
    printf("Total sum from 1 to %d using %d threads: %d\n", MAX_NUMBER, NUM_THREADS, sum);

    pthread_mutex_destroy(&sum_lock);
    return 0;
}
