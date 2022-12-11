#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#define SEGMENTSIZE 1024
#define SEGMENTPERM 0666
#define seg 64000
#define ROWLENGTH 100
#define ROWNUMBER 200

struct memory
{
    int total_segs, lines_segm, last_line;
    int segm, line;
    int test;
    char **ptr;
};
typedef struct memory *mem;

int get_key(int, int);