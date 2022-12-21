#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include "../include/timer.h"

#define SEGMENTPERM 0666 // Permissions for shared memory segment
#define seg 640000 // Maximum number of characters the segment loaded in shared memory can have
#define ROWLENGTH 1000 // Maximum length of line is 1000 characters (No line in text.txt has more characters)

struct semaphore
{
    sem_t sp, mutex;
    int num;
};
struct memory
{
    int total_segs, lines_segm, last_line;
    int segm, requests, total;
    sem_t sp1, sp2, next, mutex;
};

typedef struct semaphore *smphr;
typedef struct memory *mem;

int get_key();
char *read_file(FILE *fp);
void init(smphr sp);
void post(smphr sp);
void waits(smphr sp);
void destroy(smphr sp);