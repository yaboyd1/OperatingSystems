#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

// A simple function that combines printf and perror
int error_output(const char* message);

// A function that prints out a summation series
void summation(const size_t START, const size_t END, const size_t INC);

/* 
Consider a series, S1 = 2 + 4 + 6 + ... + 10 
and another series S2 = 1 + 3 + 5 + ... + 9
and another series S3 = 1 + 2 + 3 + ... + 10 
We know that S1 + S2 = S3.

Now write a program, where a parent process creates 2 child process to compute S1 and S2. 
And the parent process computes S3. 

The input argument for program will be the end of series number for S3. 
For example, if the execution file name is series.exe then, the argument will be
./series.exe 10 

Child 1 will compute the series from 1 to upto 10 with difference 2. 
So, it would be 1 + 3 + 5 + 7 + 9
Child 2 will compute the series from 2 to upto 10 with difference 2. 
So, it would be 2 + 4 + 6 + 8 + 10
Parent will compute the series from 1 to 10 with difference 1.
So, it would be 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10

Please show intermediate steps computed by the children and parent processes 
*/
int main(int argc, char* argv[]) {
	// Initialize variables and usage
	size_t n = 10;
	if (argc >= 2)
		sscanf(argv[1], "%lu", &n);

	// Fork first child
	pid_t child = fork();
	int status;

	// Checking if forking first child has an error
	if (child < 0)
		return error_output("Error forking first child\n");
	// First child successfully forked
	else if (child == 0) {
		printf("I am the first child. My pid is:\t%d\n", getpid());
		summation((n > 1 ? 2 : 0), n, 2);
	// Parent process (second child will be forked here)
	} else {
		// Wait for the first child to finish
		waitpid(child, &status, 0);
		
		// Fork second child
		child = fork();
		
		// Checking if forking second child has an error
		if (child < 0)
			return error_output("Error forking second child");
		// First child successfully forked
		else if (child == 0) {
			printf("I am the second child. My pid is:\t%d\n", getpid());
			
			summation((n > 0 ? 1 : 0), n, 2);
		// Back to parent process
		} else {
			// Wait for the second child to finish
			waitpid(child, &status, 0);
			printf("I am the parent. My pid is:\t\t%d\n", getpid());

			summation((n > 0 ? 1 : 0), n, 1);
		}
	}
	return 0;
}

// A simple function that combines printf and perror
int error_output(const char* message) {
	printf("%s", message);
	perror(message);
	return 1;
}

// A function that prints out a summation series
void summation(const size_t START, const size_t END, const size_t INC) {
	size_t sum = START;
	printf("%lu (%lu)", sum, sum);
	for (size_t i = START + INC; i <= END; i += INC)
		printf(" + %lu (%lu)", i, sum += i);
	printf(" = %lu\n", sum);
}