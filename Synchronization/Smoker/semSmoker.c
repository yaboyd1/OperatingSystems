#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

// Include custom semaphore library
#include "sem.h"

#define PERMISSIONS 0666 | IPC_CREAT
#define CHILD 0
#define TRUE 1

// The maximum number of times agent can produce items
#define MAXIMUM 20

int main() {
	// Create semaphores
	int 
	lock = semget(IPC_PRIVATE, 1, PERMISSIONS),
	semAgent = semget(IPC_PRIVATE, 1, PERMISSIONS),
	semTobacco = semget(IPC_PRIVATE, 1, PERMISSIONS),
	semPaper = semget(IPC_PRIVATE, 1, PERMISSIONS),
	semMatches = semget(IPC_PRIVATE, 1, PERMISSIONS);

	// Initialize semaphore values
	sem_create(lock, 1);
	sem_create(semAgent, 0);
	sem_create(semTobacco, 0);
	sem_create(semPaper, 0);
	sem_create(semMatches, 0);

	// Create children processes for producing agent and three consuming smokers
	pid_t agent, tobacco, paper, matches;

	// Create Agent Producer Process
	agent = fork();
	if (agent < 0) {
		printf("Error forking child\n");
		return 1;
	}
	else if (agent == CHILD) {
		srand(time(NULL));
		int r;
		for (int i = 0; i < MAXIMUM; ++i) {
			P(lock);

			// Agent randomly decides to produce 2 out of the 3 items
			r = rand() % 3 + 1;
			if (r == 1) {
				printf("Agent puts tobacco and paper on the table\n");
				V(semMatches); // Alow the Matches Smoker Process to run
			}
			else if (r == 2) {
				printf("Agent puts tobacco and matches on the table\n");
				V(semPaper); // Alow the Paper Smoker Process to run
			}
			else {
				printf("Agent puts paper and matches on the table\n");
				V(semTobacco); // Alow the Tobacco Smoker Process to run
			}

			V(lock);
			P(semAgent); // Wait for a Smoker Process to smoke items before next iteration
		}
	}
	else {
		// Create Tobacco Smoker Process
		tobacco = fork();
		if (tobacco < 0) {
			printf("Error forking child\n");
			return 1;
		}
		else if (tobacco == CHILD) {
			while (TRUE) {
				P(semTobacco); // Wait for Agent to produce paper and matches for Tobacco Smoker
				P(lock);

				printf("Smoker with the tobacco takes the paper and matches from the table\n");

				V(semAgent); // Allow Agent to produce more items
				V(lock);

				printf("Smoker with the tobacco smokes his cigarette\n");
			}
		}
		else {
			// Create Matches Smoker Process
			matches = fork();
			if (matches < 0) {
				printf("Error forking child\n");
				return 1;
			}
			else if (matches == CHILD) {
				while (TRUE) {
					P(semMatches); // Wait for Agent to produce tobacco and paper for Matches Smoker
					P(lock);

					printf("Smoker with the matches picks up tobacco and paper from the table\n");

					V(semAgent); // Allow Agent to produce more items
					V(lock);

					printf("Smoker with the matches smokes his cigarette\n");
				}
			}
			else {
				// Create Paper Smoker Process
				paper = fork();
				if (paper < 0) {
					printf("Error forking child\n");
					return 1;
				}
				else if (paper == CHILD) {
					while (TRUE) {
						P(semPaper); // Wait for Agent to produce tobacco and matches for Paper Smoker
						P(lock);

						printf("Smoker with the paper takes the tobacco and matches from the table\n");

						V(semAgent); // Allow Agent to produce more items
						V(lock);

						printf("Smoker with the paper smokes his cigarette\n");
					}
				}
				else {
					// Wait for the agent process and kill the other processes
					int status;
					waitpid(agent, &status, 0);
					if (WIFEXITED(status)) {
						kill(tobacco, SIGINT);
						kill(matches, SIGINT);
						kill(paper, SIGINT);
					}

					// Wait for the smoker processes
					for (int i = 0; i < 3; ++i)
						wait(&status);

					// Kill the semaphores used throughout the program
					semkill(lock);
					semkill(semAgent);
					semkill(semMatches);
					semkill(semPaper);
					semkill(semTobacco);
					return 0;
				}
			}
		}
	}
}