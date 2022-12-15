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
#define seg 640000
#define ROWLENGTH 1000
#define ROWNUMBER 200

struct semaphore
{
    sem_t sp;
    sem_t mutex;
    int i, num;
};
struct memory
{
    int total_segs, lines_segm, last_line;
    int segm, line;
    int requests;
    int waiting;
    int total, N;
    sem_t sp1, sp2, next, mutex;
    struct semaphore sp, ssp;
};

typedef struct memory *mem;
typedef struct semaphore *smphr;

int get_key(int, int);
char *read_file(FILE *fp);
int lines_per(int segm, int count);
void init(smphr sp);
void post(smphr sp);
void waits(smphr sp);
int check(smphr sp, int segm);
void waiting(smphr sp);