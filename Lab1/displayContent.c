#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// Write a C program to implement a command called displaycontent that takes a file name
// as argument and displays its contents. Report an appropiate message if the file does
// not exist or cannot be opened. You are to use read(), write(), and close() system calls.
int main(int argc, char *argv[]) {
	// Usage
	if(argc != 2) {
		printf("Improper Usage! Please provide the filepath.\n");
		return 1;
	}

	// Initialize variables
	errno = 0;
	char *filepath = argv[1], *buffer;
	int returnval, fd, count = 0;

	// Check file existence
	returnval = access(filepath, F_OK);
	if (returnval != 0) {
		if (errno == ENOENT)
			printf("%s does not exist\n", filepath);
		else if (errno == EACCES)
			printf("%s is not accessible\n", filepath);
		return 1;
	}

	// Check read access
	returnval = access(filepath, R_OK);
	if (returnval != 0) {
		printf("%s does not have read access", filepath);
		return 1;
	}

	// Open file
	fd = open(filepath, O_RDONLY);
	if(fd == -1) {
		printf("\n Open() failed with error [%s]\n", strerror(errno));
		return 1;
	}

	/* open() succeeded, now one can do read operations on the file
	since we opened it in read-only mode. Also once done with
	processing, the file needs to be closed. */

	// Read file 1 character at a time and write to stdin
	while(write(1, &buffer, read(fd, &buffer, 1)) > 0);

	// Close file
	returnval = close(fd);
	if (returnval != 0) {
		printf("Error closing the file.\n");
		return 1;
	}

	return 0;
}