#include "../include/functions.h"

int get_key(int i, int size)
{
    int shm_id;
    key_t key;
    key = ftok("shared_spac", 65);
    shm_id = shmget(key, seg * sizeof(char) + sizeof(struct memory), SEGMENTPERM | IPC_CREAT);
    if (shm_id == -1)
    {
        perror("shmget1");
        exit(EXIT_FAILURE);
    }
    return shm_id;
}