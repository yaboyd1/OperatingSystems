#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// Write a C program where open system call creates a new file (say, destination.txt) and then opens it
int main(int argc, char *argv[]) {
	char* filename = "destination.txt";
	
	if(argc == 2)
		filename = argv[1];

	// Initialize variables
	errno = 0;
	int fd = open(filename, O_RDONLY | O_CREAT);

	// Open file
	if(fd == -1) {
		printf("\n Open() failed with error [%s]\n", strerror(errno));
		return 1;
	} else {
		printf("\n Open() Successful\n");
		/* open() succeeded, now one can do read operations on the file
		since we opened it in read-only mode. Also once done with
		processing, the file needs to be closed. */
		int returnval = close(fd);
		if (returnval == 0)
			printf("File successfully closed.\n");
		else
			printf("Error closing the file.\n");
	}

	return 0;
}