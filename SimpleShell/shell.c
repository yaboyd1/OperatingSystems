#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>

int main() {
    // Initialize constants
    static const char* QUIT = "quit\n";
    static const char* DELIMITTER = "\n ";
    static const char* START = "> ";
    static const size_t SIZE = 256;

    // Inititialize variables
    char* args[SIZE];
    size_t length = 0;
    char* command;
    
    while (1) {
        printf("%s", START);

        // User input
        getline(&command, &length, stdin);

        // Stop running if user inputs "quit"
        if (strcmp(command, QUIT) == 0)
            return 0;

        // Seperate command by spaces
        length = 0;
        for (char* token = strtok(command, DELIMITTER); token != NULL; token = strtok(NULL, DELIMITTER))
            args[length++] = token;
        args[length] = NULL;

        // Execute command in a child process
        pid_t child = fork();
        if (child < 0) {
            printf("Error forking child\n");
            return 1;
        } else if (child == 0) {
            execvp(args[0], args);
            printf("%s [%s]!\n", command, strerror(errno));
            return 1;
        } else {
            wait(&child);
        }
    }

    return 0;
}