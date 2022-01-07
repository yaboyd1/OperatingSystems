#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char** argv) {
    // Must provide 1 argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <String>\n", argv[0]);
        exit(1);
    }

    // Print out the argument and sleep forever
    char *string = argv[1];
    while (1) {
        // Spin(1);
        printf("%s\n", string);
        sleep(1);
    }

    return 0;
}