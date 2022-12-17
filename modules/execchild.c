#include "../include/functions.h"
#include "../include/timer.h"

int main(int argc, char **argv, char **envp)
{
    FILE *fp;
    int id, x;
    int *seed = malloc(sizeof(int));
    double start1, end1;
    double start2, end2;
    if (argc < 2)
    {
        perror("Need more arguments");
        return EXIT_FAILURE;
    }
    id = atoi(argv[1]);

    // Make sure that every child got a different seed so they wont have the same random numbers
    time_t t = time(NULL) + id; // Different id for every child process
    *seed = (unsigned int)t;    // The argument of rand_t

    char *filename = malloc(33 * sizeof(char) + 1); // We allocate 21 bytes for "../Childrentxts/Child",4 for ".txt" and 8 more for the int id (+1 for the '\0' character)
    // Build the filename
    strcpy(filename, "../Childrentxts/Child");
    strcat(filename, argv[1]);
    strcat(filename, ".txt");
    fp = fopen(filename, "w");                                                     // Open the log file
    fprintf(fp, "This is the output file of Child process %d\n\n", atoi(argv[1])); // A title for the log file

    int shm_id = get_key(); // Get the id of the shared memory segment children share

    mem k;
    k = (mem)shmat(shm_id, NULL, 0);                                   // Attach the shared memory segment
    smphr sp = (smphr)k + sizeof(struct memory);                       // Get a pointer for the semaphore array which is located after the object 'struct memory'
    char *str = (char *)sp + k->total_segs * sizeof(struct semaphore); // Get a pointer for the string that stores text segment

    int probability = 40, random;
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
            line = 0;
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
            GET_TIME(end2);
            fprintf(fp, "Time for the request to be submitted %f\n", end1 - start1);
            fprintf(fp, "Time for the the answer to come back %f\n", end2 - start2);
            usleep(20000);
        }

        sem_wait(&(k->mutex));
        k->total++;
        sem_post(&(k->mutex));

        sem_wait(&(sp[x].mutex));
        sp[x].num--;
        if (sp[x].num == 0)
        {
            waits(&(sp[x]));
            sem_post(&(k->sp2));
        }
        sem_post(&(sp[x].mutex));
    }
    return EXIT_SUCCESS;
}