#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>

// A function that outputs to stdout and stderr given a formatted string
int error(const char *args, ...);

// Create two files with read, write, and execute permissions
int main() {
	const char* filename[] = {"destination1.txt", "destination2.txt"};
	int fd;

	for (int i = 0, N = sizeof(filename) / sizeof(filename[0]); i < N; ++i) {
		// Check if file already exists
		if (access(filename[i], F_OK) == 0) {
			// Check if file has proper read, write, and execute permissions
			if (access(filename[i], R_OK | W_OK | X_OK) < 0) {
				printf("[%s] already exists but does not have proper permissions. Deleting...\n", filename[i]);
				if (unlink(filename[i]) < 0)
					return error("Deleting [%s] failed with error [%s]\n", filename[i], strerror(errno));
			}
		}
		
		// Create file with proper read, write, and execute permissions
		if ((fd = open(filename[i], O_CREAT, 00777)) < 0)
			return error("Creating [%s] failed with error [%s]\n", filename[i], strerror(errno));

		// Close file
		if (close(fd) < 0)
			return error("Closing [%s] failed with error [%s]\n", filename[i], strerror(errno));
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