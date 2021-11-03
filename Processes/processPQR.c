#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

// Print out the relevenat variables and pid for each process
int main() {
	// Parent P
	int a = 10, b = 25, fq = 0, fr = 0;

	// Fork a child - call it Process Q
	fq = fork(); 

	// Child successfully forked
	if(fq == 0) { 
		a = a + b;
		printf("Process Q:\na: %d\nb: %d\nprocess id: %d\n", a, b, getpid());

		// Fork another child - call it Process R
		fr = fork(); 

		// Inside Parent Process Q
		if(fr != 0) {
			b = b + 20;
			printf("Parent Process Q:\na: %d\nb: %d\nprocess id: %d\n", a, b, getpid());
		// Inside Process R
		} else {
			a = (a*b) + 30;
			printf("Process R:\na: %d\nb: %d\nprocess id: %d\n", a, b, getpid());
		}
	// Inside Parent Process P
	} else {
		b = a + b - 5;
		printf("Parent Process P:\na: %d\nb: %d\nprocess id: %d\n", a, b, getpid());
	}
	return 0;
}