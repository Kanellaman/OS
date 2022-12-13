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

    int shm_id = get_key(0, 0);
    mem k;
    k = (mem)shmat(shm_id, NULL, 0);
    if (k == (mem)-1)
    {
        perror("Error attaching shared memory segment");
        return 1;
    }
    k->total_segs = segm;
    k->lines_segm = lines_per(segm, count);
    k->last_line = k->lines_segm - k->lines_segm * segm % count;
    k->segm = 10;
    k->line = 3;
    k->requests = requests;
    sem_t *sp;
    sp = (sem_t *)k + sizeof(struct memory);
    sem_init(sp, 1, 2);

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
    char **array;
    array = malloc(count * sizeof(char *));
    for (int i = 0; i < count; i++)
    {
        char *kk = read_file(fp);
        array[i] = malloc(strlen(kk) * sizeof(char) + 1);
        strcat(array[i], kk);
        // printf("%s", array[i]);
    }
    k->ptr = (char *)sp + sizeof(sem_t);

    int num = k->segm, first = (num - 1) * k->lines_segm, last;
    if (num == segm)
        last = (num - 1) * k->lines_segm + k->last_line;
    else
        last = num * k->lines_segm;
    for (int i = first; i < last; i++)
        strcat(k->ptr, array[i]);
    // printf("%s  %d\n", k->ptr, k->lines_segm);
    // exit(0);
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
        }
    }
    for (int j = 0; j < N; j++)
    {
        int i = wait(NULL);
        printf("Wait returned %d\n", i);
    }

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