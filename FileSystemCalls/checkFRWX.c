#include <stdio.h>
#include <unistd.h>
#include <errno.h>

// Check if file exists, and has read, write, and execute permissions
int main(int argc, char* argv[]) {
	// Usage
	if (argc != 2) {
		printf("Improper Usage! Please provide the filepath.\n");
		return 1;
	}

	// Initialize variables
	errno = 0;
	char* filepath = argv[1];

	// Check file existence
	int returnval = access(filepath, F_OK);
	if (returnval == 0)
		printf("%s exists\n", filepath);
	else {
		if (errno == ENOENT)
			printf("%s does not exist\n", filepath);
		else if (errno == EACCES)
			printf("%s is not accessible\n", filepath);
		return 0;
	}

	// Check read access
	returnval = access(filepath, R_OK);
	if (returnval == 0)
		printf("%s has read access\n", filepath);
	else {
		printf("%s does not have read access", filepath);
	}

	// Check write access
	returnval = access(filepath, W_OK);
	if (returnval == 0)
		printf("%s has write access\n", filepath);
	else {
		printf("%s does not have write access", filepath);
	}

	// Check execute access
	returnval = access(filepath, X_OK);
	if (returnval == 0)
		printf("%s has execute access\n", filepath);
	else {
		printf("%s does not have execute access", filepath);
	}

	return 0;
}