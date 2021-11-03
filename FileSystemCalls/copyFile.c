#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// Write a C program that mimics the cp command using open() system call to open
// source.txt in read-only mode and copy the contents of it to desination.txt using
// read() and write() system calls
int main(int argc, char *argv[]) {
	// Usage
	if(argc != 3) {
		printf("Improper Usage! Please provide the source file and destination file.\n");
		return 1;
	}

	// Initialize variables
	errno = 0;
	char* filepath[] = {argv[1], argv[2]};
	int fd[2];
	char *buffer;
	int returnval, count = 0;

	// Check existence of source file
	returnval = access(filepath[0], F_OK);
	if (returnval != 0) {
		if (errno == ENOENT)
			printf("%s does not exist\n", filepath[0]);
		else if (errno == EACCES)
			printf("%s is not accessible\n", filepath[0]);
		return 1;
	}

	// Check existence of destination file
	returnval = access(filepath[1], F_OK);
	if (returnval != 0) {
		if (errno == ENOENT)
			printf("%s does not exist. Creating %s...\n", filepath[1], filepath[1]);
		else if (errno == EACCES) {
			printf("%s is not accessible\n", filepath[1]);
			perror("Destination Access()");
			return 1;
		}
	}

	// Check read access of source file
	returnval = access(filepath[0], R_OK);
	if (returnval != 0) {
		printf("%s does not have read access", filepath[0]);
		perror("Source Assess()");
		return 1;
	}

	// Open source file
	fd[0] = open(filepath[0], O_RDONLY);
	if(fd[0] == -1) {
		printf("\n Source Open() failed with error [%s]\n", strerror(errno));
		perror("Source Open()");
		return 1;
	}

	// Open destination file with proper file permissions
	fd[1] = open(filepath[1], O_WRONLY | O_CREAT, 00664);
	if(fd[1] == -1) {
		printf("\n Destination Open() failed with error [%s]\n", strerror(errno));
		perror("Destination Open()");
		return 1;
	}

	/* open() succeeded, now one can do read operations on the file
	since we opened it in read-only mode. Also once done with
	processing, the file needs to be closed. */

	// Read source file 1 character at a time and write to destination file
	while(write(fd[1], &buffer, read(fd[0], &buffer, 1)) > 0);

	// Close source file
	returnval = close(fd[0]);
	if (returnval != 0) {
		printf("Error closing source file.\n");
		perror("Source Close()");
		return 1;
	}

	// Close destination file
	returnval = close(fd[1]);
	if (returnval != 0) {
		printf("Error closing destination file.\n");
		perror("Destination Close()");
		return 1;
	}

	return 0;
}
