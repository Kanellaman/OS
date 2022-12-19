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
        free(kk);
    }

    int shm_id = get_key();
    mem k;
    k = (mem)shmat(shm_id, NULL, 0); // Attach the shared memory segment
    if (k == (mem)NULL)
    {
        perror("Failed to attach shared memory segment");
        return EXIT_FAILURE;
    }
    k->last_line = count % segm;  // If the lines are not divided perfectly with segm then we get the lines for the last segment. If they do that its value is 0
    k->total_segs = count / segm; // Get the number of segments
    if (k->last_line != 0)        // If the lines are not divided perfectly with segm we have 1 segment more which has not been calculated by the division count/segm
        k->total_segs++;
    k->lines_segm = segm;   // The lines each segment has
    k->total = 0;           // Set the number of requests completed to 0
    k->requests = requests; // Store the number of requests each child must do so that the chil processes get the information through shared memory
    // Initialize the semaphores
    sem_init(&(k->sp1), 1, 0);
    sem_init(&(k->sp2), 1, 0);
    sem_init(&(k->next), 1, 0);
    sem_init(&(k->mutex), 1, 1);

    // Get a pointer for the semaphore array which is located after the object 'struct memory'
    smphr sp = (smphr)k + sizeof(struct memory);
    for (int i = 0; i < k->total_segs; i++)
        init(&sp[i]); // Initialize the contents of the array sp

    // Get a pointer for the string that stores text segment which is located after the array sp of 'k->total_segs' elements
    char *str = (char *)sp + k->total_segs * sizeof(struct semaphore);

    char *newargv[3] = {"execchild", NULL, NULL};
    for (int i = 0; i < N; i++)
    { // Create N children
        pid = fork();
        if (pid == 0)
        {
            char c1[11];
            sprintf(c1, "%d", i);
            newargv[1] = c1; // Set the second argumnt for child function as its id
            (void)execv("execchild", newargv);
        }
    }

    int num, first, last;
    while (k->total != N * requests) // If all requests have been served end!
    {
        strcpy(str, "\0"); // Clear the contents of variable str so that the new text segment will be stored

        sem_post(&(k->next));                              // Let a child process to choose a segment
        GET_TIME(start1);                                  // Time waiting to insert the segment
        sem_wait(&(k->sp2));                               // Wait for a child process to let you continue when it has submitted its requested segment
        num = k->segm;                                     // Get the segment we want to load to the shared memory
        first = num * k->lines_segm;                       // Get the first line
        if (num == k->total_segs - 1 && k->last_line != 0) // If we want to load the last segment and it has fewer lines than other segments, set last line to copy as the corerct one.
            last = num * k->lines_segm + k->last_line;
        else // If not, same as every other segment
            last = (num + 1) * k->lines_segm;
        for (int i = first; i < last; i++)
            strcat(str, array[i]); // Add the contents of line array[i] to str
        GET_TIME(end1);            // Time which the segment has been inserted
        GET_TIME(start2);          // Time waiting to delete the segment
        sem_post(&(k->sp1));       // Let the child process who asked the particular segment to have access to it

        sem_wait(&(k->sp2)); // Wait till no other child process need this segment so you can go to the next request
        GET_TIME(end2);      // Time the segment will be removed
        printf("Segment %d Insert time= %f and exit time= %f\n", k->segm, end1 - start1, end2 - start2);
    }

    // Wait for all the child processes to be terminated
    for (int j = 0; j < N; j++)
        wait(NULL);

    // Free the allocated memory
    for (int i = 0; i < count; i++)
        free(array[i]);
    free(array);

    sem_destroy(&(k->sp1));
    sem_destroy(&(k->sp2));
    sem_destroy(&(k->next));
    sem_destroy(&(k->mutex));
    for (int i = 0; i < k->total_segs; i++)
        destroy(&sp[i]);
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
