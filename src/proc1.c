#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <err.h>
#include "utils.h"
#include <zconf.h>
#include <time.h>

static char *readInput()
{
    char *input = malloc(sizeof(char)*255);
    if(runMode == 1)
    {
        fprintf(stdout, "Podaj dane = (max 255 znakow)\n");
    }
    fgets(input, MSG_SIZE, stdin);
    return input;
}

static void copyDataToMemory(char *data, char *memory)
{
    memcpy(memory, data, MSG_SIZE);
}

void startProc1(int semid1, int semid2, int semid3, void *memory)
{
    semlock(semid1);

    char *input = readInput();
    copyDataToMemory(input, memory);

    semunlock(semid2);
}

void startProc1withFile(int semid1, int semid2, int semid3, void *memory, char *infile)
{
    semlock(semid1);
    copyDataToMemory(infile, memory);
    semunlock(semid2);
}
