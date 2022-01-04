#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

// Global Variables
size_t THREADS = 4;
size_t RANGE = 10000;

size_t numberWithMaxDivisors = 1;
size_t maxDivisors = 1;

// Return the amount of divisors of a number
size_t countDivisors(int number);

// The function that the threads will run
void *divisors(void* paramter) {
	// Initialize constants based on parameter given
	const size_t THREAD_COUNT = *((size_t*)paramter);
	const size_t NUMBERS_PER_THREAD = RANGE / THREADS;
	const size_t START = THREAD_COUNT * NUMBERS_PER_THREAD;
	const size_t END = (THREAD_COUNT + 1) * NUMBERS_PER_THREAD;

	// printf("TC:%lu S:%lu E:%lu\n", THREAD_COUNT, START, END);

	// Iterate through values to find the number with max divisors
	size_t divisorCount;
	for (size_t number = START; number < END; ++number) {
		divisorCount = countDivisors(number);
		if (divisorCount > maxDivisors) {
			maxDivisors = divisorCount;
			numberWithMaxDivisors = number;
		}
	}

	return NULL;
}

int main(int argc, char* argv[]) {
	// Command line arguments
	if (argc > 1)
		THREADS = strtoul(argv[1], NULL, 10);
	if (argc > 2)
		RANGE = strtoul(argv[2], NULL, 10);

	if (THREADS == 0 || RANGE == 0) {
		printf("Error with arguments provided\n");
		return 1;
	}
	
	// Record when program started
	const clock_t START_TIME = clock();

	// Create a few threads
	pthread_t thread;
	for (size_t i = 0; i < THREADS; ++i) {
		size_t *arg = malloc(sizeof(*arg));
		*arg = i;
		pthread_create(&thread, NULL, divisors, arg);
	}

	// Wait for threads 
	for (size_t i = 0; i < THREADS; ++i)
		pthread_join(thread, NULL);
	
	// Figure out how much time the program took
	const clock_t END_TIME = clock();
	const double TIME_ELASPED = (double) (END_TIME - START_TIME) / CLOCKS_PER_SEC;

	// Print relavant information
	printf("Threads: %lu\nValue: %lu\nDivisors: %lu\nTime: %f seconds\n", THREADS, numberWithMaxDivisors, maxDivisors, TIME_ELASPED);
	return 0;
}

// Bruteforce algorithm to calculate the amount of divisors of a given number
size_t countDivisors(int number) {
    int limit = number;
    int numberOfDivisors = 0;

    if (number < 2) 
		return 1;

    for (int i = 1; i < limit; ++i) {
        if (number % i == 0) {
            limit = number / i;
            if (limit != i)
                numberOfDivisors++;
            numberOfDivisors++;
        }
    }

    return numberOfDivisors;
}

