#include "../include/functions.h"

int get_key()
{
    int shm_id;
    key_t key;
    key = ftok("shaq", 61);
    shm_id = shmget(key, seg * sizeof(char) + sizeof(struct memory) + 20 * sizeof(struct semaphore), SEGMENTPERM | IPC_CREAT);
    if (shm_id == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return shm_id;
}

char *read_file(FILE *fp)
{
    char *str = malloc(ROWLENGTH);
    fgets(str, ROWLENGTH, fp);
    return str;
}

void init(smphr sp)
{
    sem_init(&(sp->sp), 1, 0);
    sem_init(&(sp->mutex), 1, 1);
    sp->num = 0;
}

void post(smphr sp)
{
    sem_post(&(sp->sp));
}

void waits(smphr sp)
{
    sem_wait(&(sp->sp));
}

void destroy(smphr sp)
{
    sem_destroy(&(sp->mutex));
    sem_destroy(&(sp->sp));
}