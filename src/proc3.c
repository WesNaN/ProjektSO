#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <err.h>
#include <memory.h>
#include <zconf.h>
#include <stdint.h>
#include <sys/stat.h>
#include <ctype.h>
#include "utils.h"

static void readHexToText(char *input)
{
    for(int i = 0; i < MSG_SIZE - 1; i+=2)
    {
//        if(input[i] != '\0' && !isspace(input[i]) && input[i+1] != '\0' && !isspace(input[i+1]) && input[i] != '\n' && input[i+1] != '\n' && input[i] > 0 & input[i+1] > 0)
        if(input[i] != '\0' && input[i+1] != '\0')
        {
            char tmp[2];
            tmp[0] = input[i];
            tmp[1] = input[i+1];

            if(runMode != 1)
            {
                if (countChars == 15)
                {
                    fprintf(stdout, "%c%c\n", tmp[0], tmp[1]);
                    fflush(stdout);
                    countChars = 0;
                }
                else
                {
                    fprintf(stdout, "%c%c ", tmp[0], tmp[1]);
                    fflush(stdout);
                }
            }

            char convertedChar = (char)strtoul(tmp, NULL, 16);

            if(runMode == 1)
            {
                if (countChars == 15)
                {
                    fprintf(stdout, "%c\n", convertedChar);
                    fflush(stdout);
                    countChars = 0;
                }
                else
                {
                    fprintf(stdout, "%c", convertedChar);
                    fflush(stdout);
                }
            }
            countChars++;
        }
    }
}

void startProc3(int semid1, int semid2, int semid3)
{
    semlock(semid3);
    char *input = malloc(MSG_SIZE);
    memset(input, 0, MSG_SIZE);

    FILE *fifo = openFifo("r");
    fgets(input, MSG_SIZE, fifo);

    readHexToText(input);

    fclose(fifo);
    semunlock(semid1);
}