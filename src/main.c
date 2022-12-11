#include "../include/functions.h"
#include "../include/timer.h"

char *read_file(FILE *fp);
int lines_per(int segm, int count);

int main(int argc, char *argv[])
{
    int count = 1;
    char c;
    int N, requests, segm;
    int pid, status;
    double start, end;
    FILE *fp;
    if (argc < 3)
        return EXIT_FAILURE;
    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Couldn't open file\n");
        return EXIT_FAILURE;
    }
    N = strtol(argv[2], NULL, 10);        // Number of child-processes
    segm = strtol(argv[3], NULL, 10);     // Number of segments
    requests = strtol(argv[4], NULL, 10); // Number of requests per child process
    char *file_data;
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;
    rewind(fp);
    // for (int i = 0; i < 4; i++)
    // {
    //     int f = rand() % 100;
    //     char *kk = read_file(fp);
    //     printf("%s %d\n", kk, f);
    //     rewind(fp);
    // }
    int shm_id1 = get_key(0, 0), shm_id3 = get_key(2, 0);
    mem k;
    k = shmat(shm_id1, NULL, 0);
    if (k == (mem)-1)
    {
        perror("Error attaching shared memory segment");
        return 1;
    }
    k->total_segs = segm;
    k->lines_segm = lines_per(segm, count);
    k->last_line = k->lines_segm - k->lines_segm * segm % count;
    k->segm = 0;
    k->line = 0;
    k->test = 3;
    sem_t *sp = (sem_t *)k + sizeof(struct memory);
    sem_init(sp, 1, 2);
    int shm_id2 = get_key(1, k->lines_segm);
    k->ptr = (char **)shmat(shm_id2, NULL, 0);
    // for (int i = 0; i < k->lines_segm; i++)
    // {
    //     if ((k->ptr[i] = (char *)shmat(shm_id3, NULL, 0)) == (void *)-1)
    //     {
    //         perror("faiiiiiled");
    //         return EXIT_FAILURE;
    //     }
    // }
    // exit(0);
    // strcpy(k->ptr, "asd");
    // k->ptr = "asd";
    // printf("%s\n", k->ptr);
    // if (k->ptr = (char **)shmat(shm_id2, NULL, 0) == (void *)-1)
    // {
    //     perror("Failed to attach memory");
    //     return EXIT_FAILURE;
    // }
    // for (int i = 0; i < 10; ++i)
    // {
    // }

    // k->ptr[1] = 'a';
    // printf("GRAMMES ANA SEGM %d\n", k->lines_segm);
    // printf("TELEFTAIA GRAMMH %d\n", k->last_line);
    // printf("%ld\n", sizeof(struct memory));
    // for (int i = 0; i < 10; i++)
    // {
    //     k->ptr[i] = (char *)k + sizeof(struct memory) + (i + 1) * sizeof(char *);
    // }
    // for (int i = 0; i < 10; i++)
    //     for (int j = 0; j < 10; j++)
    //         k->ptr[i][j] = 'A';
    // printf("%s\n",k->ptr[0]);
    // k->ptr = (char **)k + sizeof(struct memory);
    // if (k->ptr == (char **)-1)
    // {
    //     perror("shmat");
    //     return 1;
    // }

    char *newargv[3] = {"yo", file_data, NULL};
    for (int i = 0; i < N; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            char c[8];
            sprintf(c, "%d", i);
            newargv[1] = c;
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
    // for (int i = 0; i < 10; i++)
    // {
    //     if (shmdt(k->ptr[i]) == -1)
    //     {
    //         perror("shmdt");
    //         return 1;
    //     }
    // }

    // Detach the array of pointers from the current process

    if (shmdt(k) == -1)
    {
        perror("shmdt");
        return 1;
    }
    if (shmctl(shm_id1, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        return 1;
    }
    if (shmctl(shm_id2, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        return 1;
    }
    if (shmctl(shm_id3, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        return 1;
    }
    fclose(fp);
    return EXIT_SUCCESS;
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
    printf("GRAMMES %d\n", lines);
    if (tmp == 0)
        return count / segm;
    lines += segm;
    return lines;
}