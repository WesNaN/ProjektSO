#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <err.h>
#include <memory.h>
#include <zconf.h>
#include <stdint.h>
#include <sys/stat.h>
#include "utils.h"

static char *convertStringToHexString(char *input)
{
    char *hex = malloc(MSG_SIZE*2);

    for(int i = 0; i < MSG_SIZE; i++)
    {
        if(input[i] != '\0')
        {
            sprintf(hex+i*2, "%02X", input[i]);
        }
    }

    return hex;
}

static void dumpMemoryToString(char *dump, char *memory)
{
    memcpy(dump, memory, MSG_SIZE);
    if(runMode == 1)
    {
        fprintf(stdout, "Data received = %s", dump);
        fflush(stdout);
    }
    memset(memory, 0, MSG_SIZE);
}

void startProc2(int semid1, int semid2, int semid3, void *memory)
{
    semlock(semid2);

    char *memoryReceived = malloc(MSG_SIZE);
    dumpMemoryToString(memoryReceived, memory);
    char *hexString = convertStringToHexString(memoryReceived);

    if(runMode == 1)
    {
        fprintf(stdout, "%s\n", hexString);
        fflush(stdout);
    }

    FILE *fifo = openFifo("w");
    fprintf(fifo, "%s", hexString);
    fclose(fifo);

    semunlock(semid3);
}




