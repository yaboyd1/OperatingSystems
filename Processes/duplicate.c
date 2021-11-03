#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define WRITE_END 1
#define READ_END 0

// A simple function that combines printf and perror
int error_output(const char* message);

/* 
Open a file called readme.txt in the child process, 
Read the contents and pass it to the parent process. 
Parent process will write the following to readme.txt, 
“Parent is writing:” and the contents it received from the child process.
We are essentially duplicating the contents of readme.txt.
HINT: Use pipe to transfer data between processes
*/
int main(int argc, char** argv) {
	// Initialize variables
	const char* message = "\nParent is writing:\n";
	char* filepath = "readme.txt";
	int pipefd[2], fd;
	char buf;

	// Usage
	if (argc < 2)
		return error_output("Too few arguments. A filepath is required\n");
	else if (argc > 2)
		error_output("Too many arguments. Only using the first\n");
	
	filepath = argv[1];

	// Check file existence
	if (access(filepath, F_OK) != 0) {
		if (errno == ENOENT)
			printf("%s does not exist\n", filepath);
		else if (errno == EACCES)
			printf("%s is not accessible\n", filepath);
		return 1;
	}

	// Check read access
	if (access(filepath, R_OK) != 0) {
		printf("%s does not have read access\n", filepath);
		return 1;
	} 

	// Check write access
	if (access(filepath, W_OK) != 0) {
		printf("%s does not have write access\n", filepath);
		return 1;
	}

	// Create the pipe
	if (pipe(pipefd) == -1)
		return error_output("Error creating pipe\n");
	
	// Fork a child
	pid_t child = fork();
	if (child < 0)
		return error_output("Error forking child\n");
	else if (child == 0) {
		// Close unused read end of the pipe
		close(pipefd[READ_END]);

		// Open file in read mode
		fd = open(filepath, O_RDONLY);

		// Read all of the text from filepath and write it to the pipe
		while (read(fd, &buf, 1) > 0)
			write(pipefd[WRITE_END], &buf, 1);
		
		close(pipefd[WRITE_END]);
	} else {
		// Wait for the child process to complete
		waitpid(child, NULL, 0);

		// Close unused write end of the pipe
		close(pipefd[WRITE_END]);  

		// Open file in write mode
		fd = open(filepath, O_WRONLY);

		// Write "Parent is writing:" on a newline
		lseek(fd, 0, SEEK_END);
		write(fd, message, strlen(message));

		// Write the text from the pipe back into filepath
		while (read(pipefd[READ_END], &buf, 1) > 0)
			write(fd, &buf, 1);
		
		close(pipefd[READ_END]);
	}

	// Close the filepath
	close(fd);

	return 0;
}

int error_output(const char* message) {
	printf("%s", message);
	perror(message);
	return 1;
}