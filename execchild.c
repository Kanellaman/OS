#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>

#define SEGMENTSIZE 1024
#define SEGMENTPERM 0666
int main(int argc, char **argv, char **envp)
{
    sem_t *sp;
    int id, err, retval;
    key_t key = ftok("shared_spac", 65);

    // printf("Geiaaa %s\n", argv[0]);

    id = shmget(key, SEGMENTSIZE, SEGMENTPERM | IPC_CREAT);
    if (id == -1)
    {
        perror("Creation");
        return EXIT_FAILURE;
    }

    int *k = (int *)shmat(id, NULL, 0);
    sp = (sem_t *)shmat(id, NULL, 0);
    if (sp == (void *)-1)
    {
        perror("Attachment.");
        return EXIT_FAILURE;
    }
    sleep(3);
    /* Initialize the semaphore. */
    retval = sem_init(sp, 1, 1);
    if (retval != 0)
    {
        perror("Couldn't initialize.");
        exit(3);
    }
    retval = sem_trywait(sp);
    printf("Did trywait. Returned %d >\n", retval);

    return 0;
}