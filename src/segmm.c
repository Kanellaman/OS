#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    if (shmctl(atoi(argv[1]), IPC_RMID, NULL) == -1)
    {
        perror("shmctl1");
        return 1;
    }
    return 0;
}