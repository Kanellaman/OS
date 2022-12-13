#include "../include/functions.h"

int get_key(int i, int size)
{
    int shm_id;
    key_t key;
    key = ftok("shared_spac", 60);
    shm_id = shmget(key, seg*sizeof(char) + sizeof(struct memory), SEGMENTPERM | IPC_CREAT);
    if (shm_id == -1)
    {
        perror("shmget1");
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

int lines_per(int segm, int count)
{
    int lines, tmp;
    lines = count / segm;
    tmp = count % segm;
    if (tmp == 0)
        return count / segm;
    lines += segm;
    return lines;
}