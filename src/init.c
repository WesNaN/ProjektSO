#include <stdio.h>
#include "proc1.h"
#include "proc2.h"
#include "proc3.h"
#include "utils.h"
#include <unistd.h>
#include <fcntl.h>

/*
 *
 *
 * Projekt SO 2018
 * Autor Jakub Wesołowski
 *
 * Wybrane sygnały: SIGINT, SIGUSR1, SIGUSR2 (S1, S2, S3)
 *
 * Ten program odpowiedzialny jest za wstępną inicjalizację
 * oraz powołanie procesów potomnych
 *
 * Program mozna odpalic w 3 roznych trybach
 *
 * a) interaktywyny     - bez paramtetrow
 * b) odczyt z pliku    - -f /sciezka/do/pliku
 * c) z urandom         - -u
 *
 */

int main(int argc, char **argv)
{
    // Parse arguments
    parseArgs(argc, argv);

    // Save parent ID;
    parentID = getpid();

    // Initiate signal actions
    initSignals();

    // Creating FIFO queue
    createQueue();

    // Creating shared memory segment
    remove("./mem");
    open("./mem", O_RDWR|O_CREAT, 0666);
    key_t key = createKey("./mem", 'K');
    memory = createSharedMemory(MSG_SIZE, key);

    // Creating semaphores for locking writing to shared memory
    semid1 = createSemaphore(".", 'A');
    semid2 = createSemaphore(".", 'B');
    semid3 = createSemaphore(".", 'C');

    // Locking semaphores
    semlock(semid2);
    semlock(semid3);

    if((childPIDS[0] = fork()) == 0)
    {
        if(runMode != 1)
        {
            while(fgets(lineBuffer, MSG_SIZE, inputFile))
            {
                if(!*suspendProcesses)
                {
                    startProc1withFile(semid1, semid2, semid3, memory, lineBuffer);
                }
            }

            // After reading file suspend process
            for(;;)
                ;
        }
        else
        {
            while(1)
            {
                if(!*suspendProcesses)
                {
                    startProc1(semid1, semid2, semid3, memory);
                }
            }
        }
    }

    if((childPIDS[1] = fork()) == 0)
    {
        while(1)
        {
            if(!*suspendProcesses)
            {
                startProc2(semid1, semid2, semid3, memory);
            }
        }
    }

    if((childPIDS[2] = fork()) == 0)
    {
        while(1)
        {
            if(!*suspendProcesses)
            {
                startProc3(semid1, semid2, semid3);
            }
        }
    }

    semunlock(semid3);
    semunlock(semid2);
    semunlock(semid1);

    // Print pids
    fprintf(stdout, "Parent: %d\nProcess 1: %d\nProcess 2: %d\nProcess 3: %d\n", parentID, childPIDS[0], childPIDS[1], childPIDS[2]);
    fflush(stdout);

    // Suspending main process
    for(;;)
        ;

}