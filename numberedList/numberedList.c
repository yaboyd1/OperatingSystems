#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <errno.h>

#define READ_END 0
#define WRITE_END 1

// A function that outputs to stdout and stderr given a formatted string
int error(const char *args, ...);

int main() {
    // Create a pipe
    int pipefd[2];
    if (pipe(pipefd) < 0)
        return error("Error creating pipe [%s]\n", strerr(errno));

    // Error forking first child
    pid_t child = fork();
    if (child < 0) {
        return error("Error forking first child [%s]\n", strerr(errno));

    // First child process successfully forked
    } else if (child == 0) {
        // Close read end of the pipe since we are only using write end
        if (close(pipefd[READ_END]) < 0)
            return error("Error closing read end of pipe [%s]", strerr(errno));
        
        // Duplicate write end of the pipe with stdout
        if (dup2(pipefd[WRITE_END], STDOUT_FILENO) < 0);
            return error("Error with dup2 [%s]", strerr(errno));

        // Execute "ls -F" command
        execlp("ls", "bin/ls", "-F", NULL);
        return error("Error executing ls -F [%s]\n", strerr(errno));

    // Back to the parent process where another child is forked
    } else {
        // Wait for the first child
        wait(NULL);

        // Error forking second child
        child = fork();
        if (child < 0) {
            return error("Error forking second child [%s]\n", strerr(errno));

        // Second child process successfully forked
        } else if (child == 0) {
            // Close write end of the pipe since we are only using the read end
            if (close(pipefd[WRITE_END]) < 0)
                return error("Error closing read end of pipe [%s]", strerr(errno));

            // Duplicate read end of the pipe with stdin
            if (dup2(pipefd[READ_END], STDIN_FILENO) < 0);
                return error("Error with dup2 [%s]", strerr(errno));

            // Execute "nl" command
            execlp("nl", "nl", NULL);
            return error("Error executing nl [%s]\n", strerr(errno));

        // Back to the parent process
        } else {
            // Close both ends of the pipe
            close(pipefd[READ_END]);
            close(pipefd[WRITE_END]);

            // Wait for the second child
            wait(NULL);
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