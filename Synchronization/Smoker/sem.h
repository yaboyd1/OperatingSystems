/*
Description:
The following library is a collection of routines for using binary semaphores in C:
    1. seminit - Initialize a semaphore.
    2. P       - Perform a P(S) (wait) operation.
    3. V       - Perform a V(S) (signal) operation.
    4. semkill - Remove a semaphore

These routines call system routines:
    1. semget - Get a semaphore
    2. semctl - Semaphore control operations
    3. semop  - Semaphore operations
                    
Complete manual entries can be obtained by:
    man semctl | col -b | lpr
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

/* This structure is used to call semctl */
union arg { 
    int val;
    struct semid_ds *buf;
    char *array;
};

/*
 * Create semaphore based on "key" parameter to "initval"
 */
void sem_create(int semid, int initval) {
    int semval;
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } s;

    s.val = initval;
    if ((semval = semctl(semid, 0, SETVAL, s)) < 0)
        printf("\n Erroe in executing semctl");
}

/*
 * Remove semaphore with semaphore id (sid) from the kernel
 */
static void semkill(sid) int sid; {
    if (semctl(sid, 0, IPC_RMID, 0) == -1)
        perror("semctl (kill)");
    printf("Semaphore with value of sid = %d is killed.\n", sid);
}

/*
 * Perform the designated "op" operation on the semaphore. If "op" is -1,
 * then this implements the "P" operation; it decrements the value of the
 * semaphore (semval) if it was >0, 
 * and blocks the caller if it was zero (semval==0)
 * If "op" is 1, then this is simply added to current value of 
 * the semaphore ("V" operation).
 */
static void semcall(sid, op) int sid; int op; {
    struct sembuf sb;

    sb.sem_num = 0; /* semaphore number within sid */
    sb.sem_op = op;
    sb.sem_flg = 0; /* blocking call */
    if (semop(sid, &sb, 1) == -1)
        perror("semop");
}

/*
 * P operation on semaphore "sid". Should be called upon entry to critical
 * region.
 */
static void P(sid) int sid; {
    semcall(sid, -1);
}

/*
 * V operation on semaphore "sid". Should be called upon exit from critical
 * region.
 */
static void V(sid) int sid; {
    semcall(sid, 1);
}