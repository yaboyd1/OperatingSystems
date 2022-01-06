#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <stdarg.h>

// A function that outputs to stdout and stderr given a formatted string
int error(const char *args, ...);

// A function that explains how to use this program from the command line to stdout
void print_usage(char* programName, char* filepath, const size_t CHAPTERS, const size_t HOMEWORKS_PER_CHAPTER, const size_t STUDENTS);

int main(int argc, char* argv[]) {
    // Default values
    size_t STUDENTS = 10, CHAPTERS = 2, HOMEWORKS_PER_CHAPTER = 2;
    char* filepath = "scores.txt";

    // User Input
    if (argc == 1) {
        if (access(filepath, F_OK | R_OK) != 0) {
            printf("No filepath provided and [%s] does not exist\n", filepath);
            printf("Consider creating [%s] or see usage below:\n", filepath);
            print_usage(argv[0], filepath, CHAPTERS, HOMEWORKS_PER_CHAPTER, STUDENTS);
            return 1;
        }
    } else {
        if (argc > 1)
            filepath = argv[1];
        if (argc > 2)
            CHAPTERS = strtoul(argv[2], NULL, 0);
        if (argc > 3)
            HOMEWORKS_PER_CHAPTER = strtoul(argv[3], NULL, 0);
        if (argc > 4)
            STUDENTS = strtoul(argv[4], NULL, 0);
        
        if (CHAPTERS < 1 || HOMEWORKS_PER_CHAPTER < 1 || STUDENTS < 1)
            return error("Invalid input. Please enter natural numbers.\n");
    }

    const size_t TOTAL = STUDENTS * CHAPTERS * HOMEWORKS_PER_CHAPTER;
    
    // Open the file in read mode
    FILE* fp = fopen(filepath, "r");
    if (fp == NULL)
        return error("Error opening the file [%s] in read mode\n", filepath);

    // Read the file score by score
    int score, index, array[TOTAL];
    for (index = 0; fscanf(fp, "%d", &score) != EOF && index < TOTAL; ++index)
        array[index] = score;
    
    // Close the file
    fclose(fp);

    // Error reading the file
    if (index != TOTAL) 
        return error("Error reading file. Please check file format of [%s]", filepath);

    // Create 'x' or 'chapter' amount of managers
    for (size_t chapter = 0; chapter < CHAPTERS; ++chapter) {
        // Error forking manager process
        pid_t manager = fork();
        if (manager < 0) {
            return 1;
        // Manager Process
        } else if (manager == 0) {
            // Create 'y' or 'homework' amount of workers
            for (size_t homework = 0; homework < HOMEWORKS_PER_CHAPTER; ++homework) {
                // Error forking worker process
                pid_t worker = fork();
                if (worker < 0) {
                    return 1;
                // Worker process
                } else if (worker == 0) {
                    // Find the average amongst the students
                    int sum = 0;
                    for (size_t student = 0; student < STUDENTS; ++student) {
                        index = student * HOMEWORKS_PER_CHAPTER * CHAPTERS + homework + chapter * HOMEWORKS_PER_CHAPTER;
                        sum += array[index];
                    }
                    printf("Chapter %lu Homework %lu Average: %f\n", chapter + 1, homework + 1, sum / (double) STUDENTS);
                    // Exit worker process early
                    return 0;
                // Back to Manager Process
                } else {
                    wait(&worker);
                }
            }
            // Exit manager process early
            return 0;
        // Back to parent or Director process
        } else {
            wait(&manager);
        }
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

// A function that explains how to use this program from the command line to stdout
void print_usage(char* programName, char* filepath, const size_t CHAPTERS, const size_t HOMEWORKS_PER_CHAPTER, const size_t STUDENTS) {
    printf("USAGE: %s [filepath]\n", programName);
    printf("USAGE: %s [filepath] [x] [y]\n", programName);
    printf("USAGE: %s [filepath] [x] [y] [z]\n", programName);
    printf("[x] - Amount of Managers or Chapters\n");
    printf("[y] - Amount of Workers or Homeworks per Chapter\n");
    printf("[z] - Amount of Students or Lines in the file\n");
    printf("DEFAULT: %s %s %lu %lu %lu\n", programName, filepath, CHAPTERS, HOMEWORKS_PER_CHAPTER, STUDENTS);
}
