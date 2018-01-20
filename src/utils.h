//
// Created by kuba on 1/6/18.
//

#ifndef PROJEKTSO_UTILS_H
#define PROJEKTSO_UTILS_H

#define MSG_SIZE     255
#define FIFO         "f_fifo"

#include <stdlib.h>
#include <bits/types/FILE.h>
#include <stdbool.h>

int childPIDS[3];
int parentID;
void *memory;
int semid1;
int semid2;
int semid3;
bool *suspendProcesses;
int runMode;
FILE *inputFile;
char *lineBuffer;
int countChars;

void* createSharedMemory(size_t size, key_t addr);
key_t createKey(char *memFile, char memKey);
int semlock(int semid);
int semunlock(int semid);
int createSemaphore(char *semFile, char charKey);
void createQueue();
FILE *openFifo(char *parm);
void initSignals();
void parseArgs(int argc, char **argv);


union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};

struct msgBuffer
{
    int size;
    char *msg;
};


#endif //PROJEKTSO_UTILS_H
