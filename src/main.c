#include "../include/functions.h"
#include "../include/timer.h"

int main(int argc, char *argv[])
{
    int count = 1;
    char c;
    int N, requests, segm;
    int pid, status;
    double start, end;
    FILE *fp;
    key_t key;
    key = ftok("shared_spac", 65);
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
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
    char **array;
    array = malloc(count * sizeof(char *));

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
    k->total_segs = segm;
    k->lines_segm = lines_per(segm, count);
    k->last_line = k->lines_segm - k->lines_segm * segm % count;
    sem_init(&(k->sp1), 1, 0);
    sem_init(&(k->sp2), 1, 0);
    k->segm = -1;
    k->line = -1;
    k->requests = requests;

    sem_t *sp = (sem_t *)k + sizeof(struct memory);
    for (int i = 0; i < segm; i++)
    {
        sem_init(&sp[i], 1, 0);
    }

    char *str = (char *)sp + segm * sizeof(sem_t);

    char *newargv[3] = {"yo", file_data, NULL};
    for (int i = 0; i < N; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            char c[8];
            sprintf(c, "%d", i);
            newargv[1] = c;
            (void)execv("execchild", newargv);
        }
    }
    sem_post(&(k->sp1));
    sem_wait(&(k->sp2));

    int num = k->segm, first = num * k->lines_segm, last;
    if (num == segm - 1)
        last = num * k->lines_segm + k->last_line;
    else
        last = (num + 1) * k->lines_segm;
    for (int i = first; i < last; i++)
        strcat(str, array[i]);
    // printf("%s %d\n", str, k->segm);
    sem_post(&sp[k->segm]);
    for (int j = 0; j < N; j++)
    {
        int i = wait(NULL);
        printf("Wait returned %d\n", i);
    }

    // if (shmdt(k) == -1)
    // {
    //     perror("shmdt");
    //     return 1;
    // }
    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
        perror("shmctl1");
        return 1;
    }
    fclose(fp);
    return EXIT_SUCCESS;
}
