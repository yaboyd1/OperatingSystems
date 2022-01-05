#include <unistd.h>
#include <stdio.h>
#include <wait.h>

#define READ_END 0
#define WRITE_END 1

int main() {
	int pipefd[2];
	pipe(pipefd);

	pid_t child1 = fork(), child2 = fork();
	if (child1 < 0 || child2 < 0) {
		return 1;
	} else if (child1 == 0 && child2 > 0) {
		close(pipefd[READ_END]);
        dup2(pipefd[WRITE_END], STDOUT_FILENO);
        execlp("ls", "bin/ls", "-F", NULL);
		return 1;
	} else if (child2 == 0 && child1 > 0) {
		close(pipefd[WRITE_END]);
		dup2(pipefd[READ_END], STDIN_FILENO);
		execlp("nl", "nl", NULL);
		return 1;
	} else {
		close(pipefd[READ_END]);
        close(pipefd[WRITE_END]);
		waitpid(child1, NULL, 0);
		waitpid(child2, NULL, 0);
	}

	return 0;
}