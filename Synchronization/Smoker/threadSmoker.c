#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#define TRUE 1

// The maximum number of times agent can produce items
#define MAXIMUM 20

// Global Variables:
// Initialize the mutext locks
pthread_mutex_t 
lock = PTHREAD_MUTEX_INITIALIZER, 
mutex_agent = PTHREAD_MUTEX_INITIALIZER,
mutex_tobacco = PTHREAD_MUTEX_INITIALIZER,
mutex_paper = PTHREAD_MUTEX_INITIALIZER, 
mutex_matches = PTHREAD_MUTEX_INITIALIZER;

// Thread IDs for agent producer thread and three smoker consumer threads
pthread_t agent, tobacco, paper, matches; 

// Functions that the threads will be running:
void *runAgent();
void *runTobacco();
void *runPaper();
void *runMatches();

// Main thread
int main() {
	// Initialize the mutexes to be locked
	pthread_mutex_lock(&mutex_agent);
	pthread_mutex_lock(&mutex_tobacco);
	pthread_mutex_lock(&mutex_paper);
	pthread_mutex_lock(&mutex_matches);

	// Create threads
	pthread_create(&agent, NULL, runAgent, NULL);
	pthread_create(&tobacco, NULL, runTobacco, NULL);
	pthread_create(&paper, NULL, runPaper, NULL);
	pthread_create(&matches, NULL, runMatches, NULL);

	// Join agent with main thread (agent already kills the three smoker threads)
	pthread_join(agent, NULL);

	return 0;
}

// Thread Function for Agent Thread
void *runAgent() {
	srand(time(NULL));
	int r;
	for (int i = 0; i < MAXIMUM; ++i) {
		pthread_mutex_lock(&lock);
		// Agent randomly decides to produce 2 out of the 3 items
		r = rand() % 3 + 1;
		if (r == 1) {
			printf("Agent puts tobacco and paper on the table\n");
			pthread_mutex_unlock(&mutex_matches); // Unlock Matches Mutex so that Matches Smoker Thread can run
		}
		else if (r == 2) {
			printf("Agent puts tobacco and matches on the table\n");
			pthread_mutex_unlock(&mutex_paper); // Unlock Paper Mutex so that Paper Smoker Thread can run
		}
		else {
			printf("Agent puts paper and matches on the table\n");
			pthread_mutex_unlock(&mutex_tobacco); // Unlock Tobacco Mutex so that Tobacco Smoker Thread can run
		}
		pthread_mutex_unlock(&lock);
		pthread_mutex_lock(&mutex_agent); // Lock Agent Mutex and wait for Smoker Threads to unlock before next iteration
	}

	// Kill the other threads because agent is finished producing
	pthread_kill(tobacco, SIGINT);
	pthread_kill(paper, SIGINT);
	pthread_kill(matches, SIGINT);
	pthread_exit(0);
}

// Thread Function for Tobacco Smoker Thread
void *runTobacco() {
	while (TRUE) {
		pthread_mutex_lock(&mutex_tobacco); // Lock Tobacco Mutex and wait for Agent Thread to unlock it again
		pthread_mutex_lock(&lock);
		printf("Smoker with the tobacco takes the paper and matches from the table\n");
		pthread_mutex_unlock(&mutex_agent); // Unlock Agent Mutex so that Agent Thread can produce more items
		pthread_mutex_unlock(&lock);
		printf("Smoker with the tobacco smokes cigarette\n");
	}
}

// Thread Function for Paper Smoker Thread
void *runPaper() {
	while (TRUE) {
		pthread_mutex_lock(&mutex_paper); // Lock Paper Mutex and wait for Agent Thread to unlock it again
		pthread_mutex_lock(&lock);
		printf("Smoker with the paper takes the tobacco and matches from the table\n");
		pthread_mutex_unlock(&mutex_agent); // Unlock Agent Mutex so that Agent Thread can produce more items
		pthread_mutex_unlock(&lock);
		printf("Smoker with the paper smokes cigarette\n");
	}
}

// Thread Function for Matches Smoker Thread
void *runMatches() {
	while (TRUE) {
		pthread_mutex_lock(&mutex_matches); // Lock Matches Mutex and wait for Agent Thread to unlock it again
		pthread_mutex_lock(&lock);
		printf("Smoker with the matches picks up tobacco and paper from the table\n");
		pthread_mutex_unlock(&mutex_agent); // Unlock Agent Mutex so that Agent Thread can produce more items
		pthread_mutex_unlock(&lock);
		printf("Smoker with the matches smokes cigarette\n");
	}
}