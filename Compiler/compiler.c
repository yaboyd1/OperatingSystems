#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>

// Program to do the following:
// gcc filename.c -o filename
// ./filename
int main(int argc, char* argv[]) {
    // Usage
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        // Initialize variables
        char* filename = argv[i]; // Just for readability
        size_t size = strlen(filename);
        char exename[size];
        strcpy(exename, filename);
        strtok(exename, "."); // Remove the extension

        pid_t child = fork();
        if (child < 0) {
            printf("Error forking child process\n");
            return 1;
        } else if (child == 0) {
            // gcc filename.c -o filename
            execlp("gcc", "gcc", filename, "-o", exename, NULL);
            printf("Error compiling %s\n", argv[i]);
            return 1;
        } else {
            // ./filename
            wait(NULL);
            pid_t child2 = fork();
            if (child2 < 0) {
                printf("Error forking child2 process\n");
                return 1;
            } else if (child2 == 0) {
                char runCommand[size];
                sprintf(runCommand, "./%s", exename);

                // printf("%s\n", runCommand);

                execlp(runCommand, runCommand, NULL);
                printf("Error running %s\n", exename);
                return 1;
            } else {
                wait(NULL);
            }
        }
    }

    return 0;
}