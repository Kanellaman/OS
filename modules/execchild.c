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
    time_t t = time(NULL) + id;
    *seed = (unsigned int)t;
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
    int probability = 30, random;
    for (int i = 0; i < k->requests; i++)
    {
        if (i == 0)
            x = rand_r(seed) % k->total_segs;
        else
        { // Choose another segment to get a line with probability of 30%
            random = rand_r(seed) % 100;
            if (random < probability)
                x = rand_r(seed) % k->total_segs;
        }
        GET_TIME(start1);
        int j = 0, line = 0, last;
        sem_wait(&(sp[x].mutex));
        sp[x].num++;
        if (sp[x].num == 1)
        {
            sem_post(&(sp[x].mutex));
            sem_wait(&(k->next));

            k->segm = x;
            sem_post(&(k->sp2));
            GET_TIME(end1);
            GET_TIME(start2);
            sem_wait(&(k->sp1));
            post(&sp[x]);
        }
        else
        {
            sem_post(&(sp[x].mutex));
            GET_TIME(end1);
            GET_TIME(start2);
            waits(&sp[x]);
            post(&sp[x]);
        }

        if (1)
        {
            GET_TIME(end2);
            fprintf(fp, "Time for the request to be submitted %f\n", end1 - start1);
            fprintf(fp, "Time for the the answer to come back %f\n", end2 - start2);
            fprintf(fp, "<%d,%d> ", x, line);
            if (k->segm == k->total_segs - 1)
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
        k->total++;
        sp[x].num--;
        printf("%d with x=%d\n", sp[x].num, x);
        if (sp[x].num == 0)
        {
            sem_post(&(k->sp2));
        }
        sem_post(&(sp[x].mutex));
    }
    printf("Process id %d %d\n", id, x);
    return EXIT_SUCCESS;
}