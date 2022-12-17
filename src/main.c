#include "../include/functions.h"
#include "../include/timer.h"

int main(int argc, char *argv[])
{
    int count = 1;
    char c;
    int N, requests, segm;
    int pid, status;
    double start1, end1, start2, end2;
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
    segm = strtol(argv[3], NULL, 10);     // Number of lines per segment
    requests = strtol(argv[4], NULL, 10); // Number of requests per child process
    char *file_data;
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;
    rewind(fp);
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
    char **array;
    array = malloc(count * sizeof(char *));

    int num, first, last;
    size_t size_segm = 0, size_sem = 0; // The bytes stored in the 2D array
    // which represents the number of bytes the document has

    for (int i = 0; i < count; i++)
    {
        char *kk = read_file(fp);
        size_segm += strlen(kk) * sizeof(char) + 1; // Add the bytes of every row
        array[i] = malloc(strlen(kk) * sizeof(char) + 1);
        strcat(array[i], kk);
    }
    int shm_id = get_key(0, 0);
    mem k;
    k = (mem)shmat(shm_id, NULL, 0);
    k->last_line = count % segm;
    k->total_segs = count / segm;
    if (k->last_line != 0)
        k->total_segs++;
    k->lines_segm = segm;
    k->segm = -1;
    k->line = -1;
    k->total = 0;
    k->N = N;
    k->count = 1;
    k->requests = requests;
    sem_init(&(k->sp1), 1, 0);
    sem_init(&(k->sp2), 1, 0);
    sem_init(&(k->next), 1, 0);
    sem_init(&(k->mutex), 1, 1);

    smphr sp = (smphr)k + sizeof(struct memory);
    for (int i = 0; i < k->total_segs; i++)
        init(&sp[i]);

    char *str = (char *)sp + k->total_segs * sizeof(struct semaphore);
    char *newargv[3] = {"yo", file_data, NULL};
    for (int i = 0; i < N; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            char c1[8], c2[8];
            sprintf(c1, "%d", N);
            newargv[0] = c1;
            sprintf(c2, "%d", i);
            newargv[1] = c2;
            (void)execv("execchild", newargv);
        }
    }

    int j = 0;
    strcpy(str, "\0");
    k->segm = -1;
    while (k->total != N * requests)
    {
        sem_post(&(k->next));
        GET_TIME(start1);
        sem_wait(&(k->sp2));
        num = k->segm;
        first = num * k->lines_segm;
        if (num == k->total_segs - 1)
            last = num * k->lines_segm + k->last_line;
        else
            last = (num + 1) * k->lines_segm;
        for (int i = first; i < last; i++)
        {
            strcat(str, array[i]);
        }
        // str[strlen(str)] = '\0';
        GET_TIME(end1);
        GET_TIME(start2);
        sem_post(&(k->sp1));

        sem_wait(&(k->sp2));
        GET_TIME(end2);
        strcpy(str, "\0");
        // printf("Segment %d Insert time= %f and exit time= %f\n", k->segm, end1 - start1, end2 - start2);
        k->segm = -1;
    }
    for (int j = 0; j < N; j++)
        wait(NULL);

    if (shmdt(k) == -1)
    {
        perror("shmdt");
        return 1;
    }
    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
        perror("shmctl1");
        return 1;
    }

    fclose(fp);
    return EXIT_SUCCESS;
}
