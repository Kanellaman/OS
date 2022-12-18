#include "../include/functions.h"
#include "../include/timer.h"

int main(int argc, char *argv[])
{
    int count = 1;
    char c;
    int N, requests, segm;
    int pid;
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
        if (c == '\n')         // Increment count if this character is newline
            count = count + 1; // We store to the variable count the number of lines the file has
    rewind(fp);                // Reset the pointer in case we need to traverse the file later

    char **array;
    array = malloc(count * sizeof(char *));
    for (int i = 0; i < count; i++)
    {                                                     // Store every line of the file in the variable array
        char *kk = read_file(fp);                         // Read line a line of the file
        array[i] = malloc(strlen(kk) * sizeof(char) + 1); // Store the line into the array
        strcat(array[i], kk);
    }

    int shm_id = get_key();
    mem k;
    k = (mem)shmat(shm_id, NULL, 0); // Attach the shared memory segment
    if (k == (mem)NULL)
    {
        perror("Failed to attach shared memory segment");
        return EXIT_FAILURE;
    }
    k->last_line = count % segm;
    k->total_segs = count / segm;
    if (k->last_line != 0)
        k->total_segs++;
    k->lines_segm = segm;
    k->segm = -1;
    k->total = 0;
    k->requests = requests;
    sem_init(&(k->sp1), 1, 0);
    sem_init(&(k->sp2), 1, 0);
    sem_init(&(k->next), 1, 0);
    sem_init(&(k->mutex), 1, 1);

    // Get a pointer for the semaphore array which is located after the object 'struct memory'
    smphr sp = (smphr)k + sizeof(struct memory);
    for (int i = 0; i < k->total_segs; i++)
        init(&sp[i]);

    // Get a pointer for the string that stores text segment which is located after the array sp of 'k->total_segs' elements
    char *str = (char *)sp + k->total_segs * sizeof(struct semaphore);

    char *newargv[3] = {"execchild", NULL, NULL};
    for (int i = 0; i < N; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            char c1[4];
            sprintf(c1, "%d", i);
            newargv[1] = c1;
            (void)execv("execchild", newargv);
        }
    }

    int num, first, last;
    while (k->total != N * requests)
    {
        // Clear the contents of variable str so that the new text segment will be stored
        strcpy(str, "\0");

        sem_post(&(k->next)); // Let a child to choose a segment
        GET_TIME(start1);
        sem_wait(&(k->sp2));
        num = k->segm;
        first = num * k->lines_segm;
        if (num == k->total_segs - 1 && k->last_line != 0)
            last = num * k->lines_segm + k->last_line;
        else
            last = (num + 1) * k->lines_segm;
        for (int i = first; i < last; i++)
            strcat(str, array[i]);
        GET_TIME(end1);
        GET_TIME(start2);
        sem_post(&(k->sp1));

        sem_wait(&(k->sp2));
        GET_TIME(end2);
        // printf("Segment %d Insert time= %f and exit time= %f\n", k->segm, end1 - start1, end2 - start2);
    }
    for (int j = 0; j < N; j++)
        wait(NULL);

    if (shmdt(k) == -1)
    { // Detach shared memory segment
        perror("shmdt");
        return 1;
    }
    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
    { // Delete shared memory segment
        perror("shmctl1");
        return 1;
    }

    fclose(fp);
    return EXIT_SUCCESS;
}
