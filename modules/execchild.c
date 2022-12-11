#include "../include/functions.h"
#include "../include/timer.h"

int main(int argc, char **argv, char **envp)
{
    FILE *fp;
    int shm_id1 = get_key(0, 0), err, retval, id, x;
    int *seed = malloc(sizeof(int));
    double start, end;
    if (argc < 2)
    {
        perror("Need more arguments");
        return EXIT_FAILURE;
    }

    id = atoi(argv[1]);
    *seed = id;
    x = rand_r(seed);
    // printf("A random number from %d %d\n", id, x);

    GET_TIME(start);
    char *filename = malloc(30 * sizeof(char) + 1);
    strcpy(filename, "../Childrentxts/Child");
    strcat(filename, argv[1]);
    strcat(filename, ".txt");
    fp = fopen(filename, "w");
    fprintf(fp, "This is the output file of Child process %d\n", atoi(argv[1]));

    mem k;
    k = shmat(shm_id1, NULL, 0);
    if (k == (mem)-1)
    {
        perror("Error attaching shared memory segment");
        return EXIT_FAILURE;
    }
    k->segm = 7;
    sem_t *sp = (sem_t *)k + sizeof(struct memory);
    sem_wait(sp);
    if (k->test == 3)
    {
        sleep(1);
        k->test++;
    }
    sem_post(sp);
    // printf("%c\n", *(k->ptr)); key = ftok("shared_segm2", 54);
    // key_t key = ftok("shared_segm2", 54);
    // int shm_id3 = shmget(key, 10 * sizeof(char), SEGMENTPERM | IPC_CREAT);
    // if (shm_id3 == -1)
    // {
    //     perror("shmget");
    //     return EXIT_FAILURE;
    // }
    // k->ptr = (char *)shmat(get_key(3), NULL, 0);
    // printf("%s\n", k->ptr);
    GET_TIME(end);
    fprintf(fp, "asddfasd %f\n", end - start);
    return EXIT_SUCCESS;
}