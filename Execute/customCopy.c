#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>

// A function that outputs to stdout and stderr given a formatted string
int error(const char *args, ...);

// Copy the contents of source.txt into destination1.txt and destination2.txt
// per the following procedure:
// 1. Read the next 100 characters from source.txt. Replace every '1' with 'L'. Write to destination1.txt
// 2. Read the next 50 characters from source.txt. Replace every '3' with 'E'. Write to destination2.txt
// 3. Repeat the previous two steps until the end of the source file.
// NOTE: The last read may not have 50 or 100 characters
int main() {
	const int N = 3;
	const char* filename[] = {"source.txt", "destination1.txt", "destination2.txt"};
	int fd[N];
	
	// Check if files exist
	for (int i = 0; i < N; ++i)
		if (access(filename[i], F_OK) < 0)
			return error("[%s] does not exist. [%s]\n", filename[i], strerror(errno));

	// Check for read permissions
	if (access(filename[0], R_OK) < 0)
		return error("[%s] does not have read permission. [%s]\n", filename[0], strerror(errno));

	// Check for write permissions
	for (int i = 1; i < N; ++i)
		if (access(filename[0], R_OK) < 0)
			return error("[%s] does not have write permission. [%s]\n", filename[i], strerror(errno));

	// Open in read mode
	if ((fd[0] = open(filename[0], O_RDONLY)) < 0)
		return error("Opening [%s] failed with error [%s]\n", filename[0], strerror(errno));
	
	// Open in write mode
	for (int i = 1; i < N; ++i)
		if ((fd[i] = open(filename[i], O_WRONLY)) < 0)
			return error("Opening [%s] failed with error [%s]\n", filename[i], strerror(errno));

	// Procedure
	static const int BUF[] = {100, 50};
	static const char FROM[] = {'1', '3'};
	static const char TO[] = {'L', 'E'};
	char buffer[BUF[0]];
	int count;
	do {
		for (int i = 0; i < 2; ++i) {
			count = read(fd[0], buffer, BUF[i]);

			// Replace characters
			for (int chr = 0; chr < count; ++chr)
				if (buffer[chr] == FROM[i])
					buffer[chr] = TO[i];

			write(fd[i + 1], buffer, count);
		}
	} while (count > 0);

	// Close files
	for (int i = 0; i < N; ++i)
		if (close(fd[i]) < 0)
			return error("Closing [%s] failed with error [%s]\n", filename[i], strerror(errno));

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