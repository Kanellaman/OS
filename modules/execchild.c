#include "../include/functions.h"
#include "../include/timer.h"

int main(int argc, char **argv, char **envp)
{
    FILE *fp;
    int err, retval, id, x;
    int *seed = malloc(sizeof(int));
    double start1, end1;
    double start2, end2;
    if (argc < 2)
    {
        perror("Need more arguments");
        return EXIT_FAILURE;
    }
    id = atoi(argv[1]);
    *seed = id;
    x = rand_r(seed);
    // printf("A random number from %d %d\n", id, x);
    char *filename = malloc(30 * sizeof(char) + 1);
    strcpy(filename, "../Childrentxts/Child");
    strcat(filename, argv[1]);
    strcat(filename, ".txt");
    fp = fopen(filename, "w");
    fprintf(fp, "This is the output file of Child process %d\n", atoi(argv[1]));

    int shm_id = get_key(0, 0);
    mem k;
    k = (mem)shmat(shm_id, NULL, 0);
    sem_t *sp = (sem_t *)k + sizeof(struct memory);
    char *str = (char *)sp + k->total_segs * sizeof(sem_t);
    // printf("%s", str);
    // GET_TIME(start1);

    // for (int i = 0; i < k->requests; ++i)
    // {
    //     sem_wait(sp);
    //     if (k->segm == -1 && k->line == -1)
    //     {
    //         // sleep(1);
    //         x = rand_r(seed) % k->total_segs;
    //         printf("Sending request from %d %d\n", id, x);
    //     }
    //     sem_post(sp);
    // }
    // printf("%s",k->ptr);
    // GET_TIME(end1);
    // int j = 0;
    // int last, segm = 4, row = 2;
    x = id;
    sem_wait(&(k->sp1));
    if (k->segm == -1)
    {
        k->segm = x;
        sem_post(&(k->sp2));
        sem_post(&(k->sp1));
        sem_wait(&sp[k->segm]);
        printf("%s %d\n", str, k->segm);
        exit(0);
    }
    else if (k->segm != -1)
    {
        sem_wait(&sp[k->segm]);
    }
    // printf("asdfasdf");exit(0);
    // k->line = 4;
    // printf("NAIP\n");
    // fprintf(fp, "Time for the request to be submitted %f\n", end1 - start1);
    // fprintf(fp, "Time for the the answer to come back %f\n", end2 - start2);
    // fprintf(fp, "<%d,%d> ", segm, row);
    // if (k->segm == k->total_segs)
    //     last = k->last_line;
    // else
    //     last = k->lines_segm;
    // for (int i = 0; i < last; ++i)
    // {
    //     while (k->ptr[j] != '\n')
    //     {
    //         if (i == k->line)
    //             fprintf(fp, "%c", k->ptr[j]);
    //         j++;
    //     }
    //     if (i == k->line)
    //         fprintf(fp, "%c", k->ptr[j]);
    //     j++;
    // }
    printf("Process id %d\n", id);
    return EXIT_SUCCESS;
}