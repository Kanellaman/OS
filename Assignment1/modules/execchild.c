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
    id = atoi(argv[1]); // Second argument given is the ID og the child process

    // Make sure that every child got a different seed so they wont have the same random numbers
    time_t t = time(NULL) + id; // Different id for every child process
    *seed = (unsigned int)t;    // The argument of rand_t

    char *filename = malloc(36 * sizeof(char) + 1); // We allocate 21 bytes for "../Childrentxts/Child",4 for ".txt" and 10 more for the int id (+1 for the '\0' character)
    // Build the filename for the childs' file log
    strcpy(filename, "../Childrentxts/Child"); // Be aware that in order for the program to work the folder Childrentxt must already exists!
    strcat(filename, argv[1]);
    strcat(filename, ".txt");
    fp = fopen(filename, "w");                                                     // Open the log file
    fprintf(fp, "This is the output file of Child process %d\n\n", atoi(argv[1])); // A title for the log file

    int shm_id = get_key(); // Get the id of the shared memory segment children share

    mem k;
    k = (mem)shmat(shm_id, NULL, 0); // Attach the shared memory segment
    if (k == (mem)NULL)
    {
        perror("Failed to attach shared memory segment");
        return EXIT_FAILURE;
    }
    smphr sp = (smphr)k + sizeof(struct memory);                       // Get a pointer for the semaphore array which is located after the object 'struct memory'
    char *str = (char *)sp + k->total_segs * sizeof(struct semaphore); // Get a pointer for the string that stores text segment which is located after the array sp of 'k->total_segs' elements

    int probability = 40, random;
    for (int i = 0; i < k->requests; i++)
    {
        // Get the segment and the line you will get
        if (i == 0)
            x = rand_r(seed) % k->total_segs; // For the first request get completely randomly the segment
        else
        { // Choose another segment to get a line with probability of 40%
            random = rand_r(seed) % 100;
            if (random < probability)
                x = rand_r(seed) % k->total_segs;
        }
        int line;
        // Depending on the segment you get, get also a line
        if (x == k->total_segs - 1 && k->last_line != 0) // If x is the last segment we have to check if it has less lines than the other segments
            line = rand_r(seed) % k->last_line;
        else
            line = rand_r(seed) % k->lines_segm;
        GET_TIME(start1); // Time that the request is ready and is waiting to be submitted

        sem_wait(&(sp[x].mutex)); // Protect the variable sp[x].num!!
        sp[x].num++;              // Add the counter sp[x].num which indicates how many child processes need the segment x!
        if (sp[x].num == 1)       // If you are the first process asking for this segment...
        {
            sem_post(&(sp[x].mutex)); // No need to protect sp[x].num anymore
            sem_wait(&(k->next));     // ...wait with the other "firsts" who ask for a segment

            k->segm = x;
            sem_post(&(k->sp2));
            GET_TIME(end1);      // Request submitted
            GET_TIME(start2);    // Start waiting for the mother proceess to load the segment
            sem_wait(&(k->sp1)); // Wait for the segment to be loaded to the shared memory
        }
        else // If you are not the first process asking for this segment...
        {
            sem_post(&(sp[x].mutex)); // No need to protect sp[x].num anymore
            GET_TIME(end1);           // Request submitted
            GET_TIME(start2);         // Start waiting for the mother proceess to load the segment
            waits(&sp[x]);            //...wait for the first child process or another child process like you who asked the same segment as you to tell you when the segment is ready!
        }
        GET_TIME(end2); // Time that mother process has loaded the segment
        post(&sp[x]);   // Give access to the child process waiting for the same segment (every child process which is being freed is giving access to the next one waiting etc...)

        if (1)
        {
            fprintf(fp, "<%d,%d> ", x, line);
            int j = 0, last;
            if (x == k->total_segs - 1 && k->last_line != 0) // If x is the last segment we have to check if it has less lines than the other segments
                last = k->last_line;
            else
                last = k->lines_segm;
            for (int i = 0; i < last; ++i)
            { // Traverse every line of the string str till we get the line we want
                while (str[j] != '\n')
                {
                    if (i == line) // If you find the line you have been looking for print its contents
                        fprintf(fp, "%c", str[j]);
                    j++;
                }
                if (i == line) // If you find the line you have been looking for print its last character too which will be '\n'
                    fprintf(fp, "%c", str[j]);
                j++;
            }
            fprintf(fp, "Time for the request to be submitted %f\n", end1 - start1);
            fprintf(fp, "Time for the answer to come back %f\n", end2 - start2);
        }

        sem_wait(&(k->mutex)); // Protect variable k->total!!
        k->total++;            // This the counter of the requests that have been served and is used for mother process to quit serving more
        sem_post(&(k->mutex)); // No need to protect k->total anymore

        sem_wait(&(sp[x].mutex)); // Protect the variable sp[x].num!!
        sp[x].num--;              // Decrease the counter by 1 as the child process does not need the segment x no more
        if (sp[x].num == 0)       // If there is no other child process that needs x segment....
        {
            waits(&(sp[x]));     // Close the "door" for the next process who wants x segment (1 is always open when x is already loaded)
            sem_post(&(k->sp2)); // Give signal to mother process to serve an other request
        }
        sem_post(&(sp[x].mutex)); // No need to protect sp[x].num anymore
        usleep(20000);
    }
    return EXIT_SUCCESS;
}