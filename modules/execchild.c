#include "../include/functions.h"
#include "../include/timer.h"

int main(int argc, char **argv, char **envp)
{
    FILE *fp;
    int shm_id1 = get_key(0, 0), err, retval, id, x;
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

    mem k;
    sem_t *sp;

    k = shmat(shm_id1, NULL, 0);
    if (k == (mem)-1)
    {
        perror("Error attaching shared memory segment");
        return EXIT_FAILURE;
    }
    sp = (sem_t *)k + sizeof(struct memory);
    if (sp == (sem_t *)-1)
    {
        perror("Error attaching shared memory segment");
        return EXIT_FAILURE;
    }
    k->ptr = (char *)sp + sizeof(sem_t);
    if (k->ptr == (char *)-1)
    {
        perror("Error attaching shared memory segment");
        return EXIT_FAILURE;
    }

    GET_TIME(start1);
    for (int i = 0; i < k->requests; ++i)
    {
        sem_wait(sp);
        if (k->segm == -1 && k->line == -1)
        {
            // sleep(1);
            x = rand_r(seed) % k->total_segs;
            printf("Sending request from %d %d\n", id, x);
        }
        sem_post(sp);
    }
    GET_TIME(end1);
    int j = 0;
    int last, segm = 4, row = 2;
    
    if (k->segm != -1)
    {
        fprintf(fp, "Time for the request to be submitted %f\n", end1 - start1);
        fprintf(fp, "Time for the the answer to come back %f\n", end2 - start2);
        fprintf(fp, "<%d,%d> ", segm, row);
        if (k->segm == k->total_segs)
            last = k->last_line;
        else
            last = k->lines_segm;
        for (int i = 0; i < last; ++i)
        {
            while (k->ptr[j] != '\n')
            {
                if (i == k->line)
                    fprintf(fp, "%c", k->ptr[j]);
                j++;
            }
            if (i == k->line)
                fprintf(fp, "%c", k->ptr[j]);
            j++;
        }
        sleep(0.02);
    }
    printf("%d", k->lines_segm);
    return EXIT_SUCCESS;
}