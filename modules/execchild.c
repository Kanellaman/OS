#include "../include/functions.h"
#include "../include/timer.h"

int main(int argc, char **argv, char **envp)
{
    FILE *fp;
    int err, retval, id, x, N;
    int *seed = malloc(sizeof(int));
    double start1, end1;
    double start2, end2;
    if (argc < 2)
    {
        perror("Need more arguments");
        return EXIT_FAILURE;
    }
    N = atoi(argv[0]);
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
    smphr sp = (smphr)k + sizeof(struct memory);
    char *str = (char *)sp + k->total_segs * sizeof(struct semaphore);

    int j = 0, line = 0, last;
    x = id % 4;
    while (1)
    {
        int sum = 0;
        sem_wait(&(sp[x].mutex));
        sp[x].num++;
        sem_post(&(sp[x].mutex));
        for (int i = 0; i < k->total_segs; i++)
        {
            sum += sp[i].num;
        }
        printf("%d SUM\n", sum);
        if (sum == k->N)
            sem_post(&(k->sp2));

        waits(&(k->sp));
        if (k->segm == -1)
        {
            k->segm = x;
            sem_post(&(k->sp2));
            for (int i = 0; i < k->N - 1; i++)
                post(&(k->sp));
            break;
        }
        else if (k->segm == x)
        {
            break;
        }
        else
        {
            waits(&(k->ssp));
            // return EXIT_SUCCESS;
        }
    }
    waits(&(sp[x]));
    post(&(sp[x]));

    if (1)
    {
        fprintf(fp, "Time for the request to be submitted %f\n", end1 - start1);
        fprintf(fp, "Time for the the answer to come back %f\n", end2 - start2);
        fprintf(fp, "<%d,%d> ", x, line);
        if (k->segm == k->total_segs)
            last = k->last_line;
        else
            last = k->lines_segm;
        for (int i = 0; i < last; ++i)
        {
            while (str[j] != '\n')
            {
                if (i == line)
                    fprintf(fp, "%c", str[j]);
                j++;
            }
            if (i == line)
                fprintf(fp, "%c", str[j]);
            j++;
        }
        usleep(20000);
    }

    sem_wait(&(sp[x].mutex));
    sp[x].num--;
    k->N--;
    k->total++;
    printf("SP[X=%d] NUM=%d\n", x, sp[x].num);
    if (sp[x].num == 0)
    {
        k->segm = -1;
        strcpy(str, "\0");
        for (int i = 0; i < k->total_segs; i++)
            sp[i].num = 0;
        while (k->ssp.i < 0)
            post(&(k->ssp));
    }
    sem_post(&(sp[x].mutex));

    printf("Process id %d %d\n", id, x);
    return EXIT_SUCCESS;
}