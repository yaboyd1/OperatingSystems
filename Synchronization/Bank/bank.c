#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

// Include custom semaphore library
#include "sem.h"

#define CHILD                   0              // Return value of child process from fork call
#define TRUE                    1              // True
#define FALSE                   0              // False

#define NumOfDepositAttempt     5              // How many times Dad will deposit money
#define NumOfWithdrawAttempt    20             // Total amount of attempts SON_1 and SON_2 has
#define DepositFile             "deposit.txt"  // File Path to store deposit attempts
#define WithdrawFile            "withdraw.txt" // File Path to store withdraw attempts

#define DepositAmount           60             // Amount of money Dad deposits at a time
#define WithdrawAmount          20             // Amount of money Son withdraws at a time
#define initBalance             0              // Initial Balance
#define BalanceFile             "balance.txt"  // File Path to store balance

#define SLEEP_TIME              1              // How long processes will sleep

FILE *fp1, *fp2, *fp3, *fp4, *fp5; // File Pointers

int main() {
	int pid;        // Process ID after fork call
	int i;          // Loop indices
	int N;          // Number of times dad does update
	int N_Att;      // Number of time sons allowed to do update
	int D_Att;      // Number of times dad allowed to do update
	int status;     // Exit status of child process
	int balance;    // Balance read by processes
	
	srand(time(NULL));

	// Create Semaphore Mutex
	int semMutex = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	if (semMutex < 0) {
		perror("Semaphore failed\n");
		exit(1);
	}
	sem_create(semMutex, 1);

	// Create Semaphore Full
	int semFull = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	if (semFull < 0) {
		perror("Semaphore failed\n");
		exit(1);
	}
	sem_create(semFull, 0);

	// Initialize the file balance to be $0
	fp1 = fopen(BalanceFile, "w+");
	fprintf(fp1, "%d\n", initBalance);
	fclose(fp1);

	// Initialize the number of withdraw attempts 
	fp4 = fopen(WithdrawFile, "w+");
	N_Att = NumOfWithdrawAttempt;
	fprintf(fp4, "%d\n", N_Att);
	fclose(fp4);

	// Initialize the number of deposit attempts 
	fp5 = fopen(DepositFile, "w+");
	D_Att = NumOfDepositAttempt;
	fprintf(fp5, "%d\n", D_Att);
	fclose(fp5);

	// Create child processes that will do the updates
	if ((pid = fork()) == -1) {
		// Fork failed!
		perror("fork");
		exit(1);
	}
	if (pid == CHILD) {
		// First Child Process. Dad tries to do some updates.
		printf("Dad's Pid: %d\n", getpid());
		N = NumOfDepositAttempt;
		for (i = 1; i <= N; i++) {
			P(semMutex);

			printf("Dad is requesting to view the balance.\n"); // Dad is requesting to get hold of an ATM.
			fp1 = fopen(BalanceFile, "r+"); // Dad successfully got hold of the ATM.
			fp5 = fopen(DepositFile, "r+"); // Dad successfully got hold of deposit attempts
			fscanf(fp1, "%d", &balance);
			printf("Dad reads balance = %d \n", balance);
			int r = rand() % SLEEP_TIME + 1;
			printf("Dad needs %d sec to prepare money\n", r);

			sleep(r); // Dad Process is sleeping for r sec. You need to make sure that other processes can work in the mean time.

			fseek(fp1, 0L, 0); // Dad will now deposit the money. For this Dad will access the ATM again. And update the current balance.
			fseek(fp5, 0L, 0);
			balance += DepositAmount;
			fprintf(fp1, "%d\n", balance);
			fprintf(fp5, "%d\n", N - i);
			fclose(fp1);
			fclose(fp5);
			printf("Dad writes new balance = %d \n", balance);
			printf("Dad will deposit %d more time(s)\n", N - i); // Dad deposited the money. 

			// Increment semFull by 3
			for (int j = 0, MAX = DepositAmount / WithdrawAmount; j < MAX; ++j)
				V(semFull);

			V(semMutex);

			sleep(rand() % SLEEP_TIME + 1); // Dad will wait some time before requesting to see balance again.
		}
	} else {
		// Parent Process. Fork off another child process.
		if ((pid = fork()) == -1) {
			// Fork failed!
			perror("fork");
			exit(1);
		}
		if (pid == CHILD) {
			printf("First Son's Pid: %d\n", getpid());
			// Second child process. First poor son tries to do updates.
			while (1) {
				P(semFull);
				P(semMutex);

				printf("SON_1 is requesting to view the balance.\n"); // Son_1 is requesting to get hold of the ATM.
				fp3 = fopen(WithdrawFile, "r+"); // Son_1 successfully got hold of the ATM.
				fscanf(fp3, "%d", &N_Att); // Son_1 Checks if he has more than 0 attempt remaining.
				printf("Attempt remaining: %d.\n", N_Att);
				if (N_Att == 0) {
					fclose(fp3);
					V(semMutex);
					exit(0);
				} else {
					fp2 = fopen(BalanceFile, "r+"); // Son_1 reads the balance. 
					fscanf(fp2, "%d", &balance);
					printf("SON_1 reads balance. Available Balance: %d \n", balance);
					printf("SON_1 wants to withdraw money. "); // And if balance is greater than Withdraw amount, then son can withdraw money.
					if (balance < WithdrawAmount) {
						printf("There is not enough balance!\n");
						fclose(fp2);
						fclose(fp3);
						fp5 = fopen(DepositFile, "r+");
						fscanf(fp5, "%d", &D_Att);
						if (D_Att == 0) {
							V(semMutex);
							exit(0);
						}
					} else {
						fseek(fp2, 0L, 0);
						balance -= WithdrawAmount;
						fprintf(fp2, "%d\n", balance);
						fclose(fp2);

						printf("SON_1 withdrawed %d. New Balance: %d \n", WithdrawAmount, balance);
						fseek(fp3, 0L, 0); // SON_1 will write the number of  attempt remaining in the attampt.txt file.
						N_Att -= 1;
						fprintf(fp3, "%d\n", N_Att);
						fclose(fp3);
						printf("Number of attempts remaining: %d \n", N_Att);
					}
				}
				V(semMutex);
				V(semFull);

				sleep(rand() % SLEEP_TIME + 1); // SON_1 will wait some time before the next request.
			}
		} else {
			// Parent Process. Fork off one more child process.
			if ((pid = fork()) == -1) {
				// Fork failed!
				perror("fork");
				exit(1);
			}
			if (pid == CHILD) {
				printf("Second Son's Pid: %d\n", getpid());
				// Third child process. Second poor son tries to do updates.
				while (1) {
					P(semFull);
					P(semMutex);

					printf("SON_2 is requesting to view the balance.\n"); // Son_2 is requesting to get hold of the ATM.
					fp3 = fopen(WithdrawFile, "r+"); // Son_2 successfully got hold of the ATM.
					fscanf(fp3, "%d", &N_Att); // Son_2 Checks if he has more than 0 attempt remaining.
					printf("Attempt remaining: %d.\n", N_Att);
					if (N_Att == 0) {
						fclose(fp3);
						V(semMutex);
						exit(0);
					} else {
						fp2 = fopen(BalanceFile, "r+"); // Son_2 reads the balance. 
						fscanf(fp2, "%d", &balance);
						printf("SON_2 reads balance. Available Balance: %d \n", balance);
						printf("SON_2 wants to withdraw money. "); // And if balance is greater than Withdraw amount, then son can withdraw money.
						if (balance < WithdrawAmount) {
							printf("There is not enough balance!\n");
							fclose(fp2);
							fclose(fp3);
							fp5 = fopen(DepositFile, "r+");
							fscanf(fp5, "%d", &D_Att);
							if (D_Att == 0) {
								V(semMutex);
								exit(0);
							}
						} else {
							fseek(fp2, 0L, 0);
							balance -= WithdrawAmount;
							fprintf(fp2, "%d\n", balance);
							fclose(fp2);

							printf("SON_2 withdrawed %d. New Balance: %d \n", WithdrawAmount, balance);
							fseek(fp3, 0L, 0); // SON_2 will write the number of  attempt remaining in the attampt.txt file.
							N_Att -= 1;
							fprintf(fp3, "%d\n", N_Att);
							fclose(fp3);
							printf("Number of attempts remaining: %d \n", N_Att);
						}
					}
					V(semMutex);
					V(semFull);

					sleep(rand() % SLEEP_TIME + 1); // SON_2 will wait some time before the next request.
				}
			} else {
				// Now parent process waits for the child processes to finish
				pid = wait(&status);
				printf("Child(pid = %d) exited with the status %d. \n", pid, status);

				pid = wait(&status);
				printf("Child(pid = %d) exited with the status %d. \n", pid, status);

				pid = wait(&status);
				printf("Child(pid = %d) exited with the status %d. \n", pid, status);

				// Kill semaphores since we are no longer using them
				semkill(semMutex);
				semkill(semFull);
			}
			exit(0);
		}
		exit(0);
	}
	exit(0);
}