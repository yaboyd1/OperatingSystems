#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

// Write a program where the parent process produce two child processes. 
// Print the first child and its pid. Next print the second child and its pid.
// Guarantee that the parent terminates after the children terminate.
// (Note, you need to waitfor two child processes here). 
// Use the getpid() function to retrieve the PID.
int main() {
	// Initialize variables and fork first child
	pid_t child = fork();
	int status;

	// Checking if forking first child has an error
	if (child < 0) {
		printf("Error forking first child\n");
		perror("Error forking first child1\n");
		return 1;
	// First child successfully forked
	} else if (child == 0) {
		printf("I am the first child. My pid is:\t%d\n", getpid());
	// Parent process (second child will be forked here)
	} else {
		// Wait for the first child to finish
		waitpid(child, &status, 0);
		
		// Fork second child
		child = fork();
		
		// Checking if forking second child has an error
		if (child < 0) {
			printf("Error forking second child\n");
			perror("Error forking second child\n");
			return 1;
		// First child successfully forked
		} else if (child == 0) {
			printf("I am the second child. My pid is:\t%d\n", getpid());
		// Back to parent process
		} else {
			// Wait for the second child to finish
			waitpid(child, &status, 0);
			printf("I am the parent. My pid is:\t\t\t%d\n", getpid());
		}
	}
	return 0;
}