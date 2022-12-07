#include <stdio.h>
#include <stdlib.h>
// #include "functions.h"
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/wait.h>

#define SEGMENTSIZE 1024
#define SEGMENTPERM 0666
char *read_file(FILE *fp)
{

    int max = 256;
    char *str = malloc(max * sizeof(char) + 1);
    fgets(str, max, fp);
    return str;
}
int k = 432;

int main(int argc, char *argv[])
{
    int N, requests,segm;
    int pid, status;
    FILE *fp;
    if (argc < 3)
        return EXIT_FAILURE;
    fp = fopen(argv[1], "r");
    N = strtol(argv[2], NULL, 10); // Number of child-processes
    segm=strtol(argv[3],NULL,10); // Number of segments
    requests = strtol(argv[4], NULL, 10); // Number of requests per child process

    if (fp == NULL)
    {
        printf("Couldn't open file\n");
        return EXIT_FAILURE;
    }

    // for (int i = 0; i < 4; i++)
    // {
    //     int f = rand() % 100;
    //     kk = read_file(fp);
    //     printf("%s %d\n", kk, f);
    //     rewind(fp);
    // }
    // printf("Test");

    sem_t *sp;
    key_t key = ftok("shared_spac", 65);
    int shm_id;
    shm_id = shmget(key, SEGMENTSIZE, SEGMENTPERM | IPC_CREAT);
    if (shm_id == -1)
    {
        perror("Creation");
        return EXIT_FAILURE;
    }
    sp = (sem_t *)shmat(shm_id, NULL, 0);
    char *newargv[3] = {"yo"};

    for (int i = 0; i < N; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            srand(pid);
            (void)execv("execchild", newargv);
            sleep(rand() % 4);
            exit(0);
        }
    }
    for (int j = 0; j < N; j++)
    {
        int i = wait(NULL);
        printf("Wait returned %d\n", i);
    }
    fclose(fp);
    shmctl(shm_id, IPC_RMID, NULL);
    return EXIT_SUCCESS;
}