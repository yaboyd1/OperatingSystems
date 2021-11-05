#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>

// Write a progream where a child is create to execute a command.
// The command will list all files in a directory (including hidden files)
// with information including permissions, owner, size, and when last modified. 
// Use execvp().
// HINT: Pass additional arguments to the command
// Announce the successful forking of a child by displaying its pid. 
int main() {
    pid_t child = fork();

    // Error forking child
    if (child < 0) {
        printf("Error forking child. [%s]\n", strerror(errno));
        perror("Error forking child");
        return 1;
    // Forked child
    } else if (child == 0) {
        printf("Child successfully forked with process id [%d]\n", getpid());
        
        // Executing ls -la
        char* args[] = {"ls", "-la", NULL};
        execvp("ls", args);

        printf("execl failed. [%s]\n", strerror(errno));
        return 1;
    // Parent process
    } else {
        wait(&child);
        printf("Back to the Parent process with process id [%d]\n", getpid());
    }
    return 0;
}