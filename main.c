#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include <sys/types.h>
#include <string.h>

char *read(FILE *fp)
{

    int max = 256;
    char *str = malloc(max * sizeof(char) + 1);
    fgets(str, max, fp);
    return str;
}

int main(int argc, char *argv[])
{
    printf("%s\n", argv[1]);
    FILE *fp;
    if (argc < 2)
    {
        return 1;
    }
    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Couldn't open file\n");
        return 1;
    }

    char *kk;
    for (int i = 0; i < 290; i++)
    {
        int f = rand();

        kk = read(fp);
        printf("%s %d\n", kk, f);
    }
    fclose(fp);
    return 0;
}