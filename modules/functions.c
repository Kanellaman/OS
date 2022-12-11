#include "../include/functions.h"

int get_key(int i, int size)
{
    int shm_id;
    key_t key;
    if (i == 0)
    {
        key = ftok("shared_spac", 65);
        shm_id = shmget(key, 1000, SEGMENTPERM | IPC_CREAT);
        if (shm_id == -1)
        {
            perror("shmget");
            exit(EXIT_FAILURE);
        }
    }
    else if (i == 1)
    {
        key = ftok("shared_segm", 60);
        shm_id = shmget(key,150 * sizeof(char *), SEGMENTPERM | IPC_CREAT);
        if (shm_id == -1)
        {
            perror("shmget");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        key = ftok("shared_segm2", 54);
        shm_id = shmget(key, ROWLENGTH * sizeof(char), SEGMENTPERM | IPC_CREAT);
        if (shm_id == -1)
        {
            perror("shmget");
            exit(EXIT_FAILURE);
        }
    }
    return shm_id;
}