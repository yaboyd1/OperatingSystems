#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// Repeat as per the following procedure:
// 1. Read the next 100 characters from source.txt and among the characters read,
// replace each character '1' with character 'L' and all characters are then written in 
// destination.txt
// 2. Write characters "XYZ" into file destination.txt
// 3. Repeat the previous steps until the end of file source.txt. The last read step may not have
// have 100 characters
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
	int returnval;

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
			return 1;
		}
	} 

	// Check read access of source file
	returnval = access(filepath[0], R_OK);
	if (returnval != 0) {
		printf("%s does not have read access", filepath[0]);
		return 1;
	}

	// Open source file
	fd[0] = open(filepath[0], O_RDONLY);
	if(fd[0] == -1) {
		printf("Source Open() failed with error [%s]\n", strerror(errno));
		return 1;
	}

	// Open destination file with proper file permissions
	fd[1] = open(filepath[1], O_WRONLY | O_CREAT, 00664);
	if(fd[1] == -1) {
		printf("Desination Open() failed with error [%s]\n", strerror(errno));
		return 1;
	}

	// Initialize variables
	static const int BUFFER_SIZE = 100;
	int count = 0;
	char* buffer;

	// Read every 100 characters
	while ((count = read(fd[0], buffer, BUFFER_SIZE)) > 0) {
		// Check to make sure there are no errors
		if (count < 0) {
			printf("Source Read() with error [%s]\n", strerror(errno));
			perror("Source Read()");
			return 1;
		}

		// Replace every 1 with an L
		for (int i = 0; i < count; ++i)
			if (buffer[i] == '1') 
				buffer[i] = 'L';

		// Write new string with every 1 replaced with L into file
		if (write(fd[1], buffer, count) < 0) {
			printf("Desination Write() with error [%s]\n", strerror(errno));
			perror("Destination Write()");
			return 1;
		}

		// Write XYZ into file
		if (write(fd[1], "XYZ", 3) < 0) {
			printf("Desination Write() with error [%s]\n", strerror(errno));
			perror("Destination Write()");
			return 1;
		}
	}

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