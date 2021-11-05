#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include <stdarg.h>

// A function that outputs to stdout and stderr given a formatted string
int error(const char *args, ...);

// Create a child process that creates 2 files with the proper permissions
// After this child is complete, create another child process that executes
// the custom copy procedure 
int main() {
	char* exename[] = {"./createFilesRWX", "./customCopy"};
	pid_t child = fork();

	// Error forking first child
	if (child < 0)
		return error("Error forking child [%s]\n", strerror(errno));
	
	// First child forked
	else if (child == 0) {
		execl(exename[0], exename[0], NULL);
		return error("Error running first child process [%s]\n", strerror(errno));
	} else {
		wait(&child);
		child = fork();

		// Error forking second child
		if (child < 0)
			return error("Error forking child [%s]\n", strerror(errno));
		
		// Second child forked
		else if (child == 0) {
			execl(exename[1], exename[1], NULL);
			return error("Error running second child process [%s]\n", strerror(errno));

		// Parent process
		} else {
			wait(&child);
			printf("Parent Process with process id [%d]\n", getpid());
		}
	}
	return 0;
}

// A function that outputs to stdout and stderr given a formatted string
int error(const char *args, ...) {
	va_list ap;
	
	// Print to standard output
	va_start(ap, args);
	vprintf(args, ap);
	va_end(ap);
	
	// Print to standard error output
	va_start(ap, args);
	vfprintf(stderr, args, ap);
	va_end(ap);

	return 1;
}