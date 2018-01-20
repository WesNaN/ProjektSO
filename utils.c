//
// Created by kuba on 1/6/18.
//

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/mman.h>
#include <memory.h>

void* createSharedMemory(size_t size, key_t addr)
{
    int id;
    if((id = shmget(addr, size, IPC_CREAT | 0666)) == -1)
    {
        errx(1, "Error while creating shared memory segment!");
    }

    char *mem;
    if((mem = shmat(id, NULL, 0)) == (char*) -1)
    {
        errx(2, "Error while creating pointer to shared mem segment!");
    }
    return mem;
}

key_t createKey(char *memFile, char memKey)
{
    key_t key;
    if ((key = ftok(memFile, memKey)) == - 1)
    {
        errx(1, "Eror while creating shared memory key!");
    }
    return key;
}

int semunlock(int semid) {

    /* Struktura opisujaca operacje na semaforach */
    struct sembuf opr;

    opr.sem_num = 0;
    opr.sem_op  = 1;
    opr.sem_flg = 0;

    if (semop(semid, &opr, 1) == -1){
        warn("Blad odblokowania semafora!");
        return 0;
    }else{
        return 1;
    }
}

int semlock(int semid) {

    /* Struktura opisujaca operacje na semaforach */
    struct sembuf opr;
    /* podgld struktury z opisem pol:
         struct sembuf {
            ushort semnum;
            short sem_op;
            ushort sem_flg;
        };
    *
    * gdzie:
    * 	semnum - numer semafora,
    * 	sem_op - operacja na semaforze:
    * 		sem_op > 0 - (V) zwiekszenie semafora o wartosc "sem_op"
    * 		sem_op < 0 - (P) polozenie semafora (wstrzymanie procesu)
    * 					 lub zmniejszenie semafora o wartosc "sem_op"
    * 		sem_op = 0 - (Z) "przejscie pod semaforem", odwrotnosc (P)
    */

    opr.sem_num =  0;
    opr.sem_op  = -1;        // blokowanie
    opr.sem_flg =  0;        // operacja blokujaca

    if (semop(semid, &opr, 1) == -1){
        warn("Blad blokowania semafora!");
        return 0;
    }else{
        return 1;
    }
}

int createSemaphore(char *semFile, char charKey)
{
    union semun ctl;
    int semid;
    key_t key = createKey(semFile, charKey);

    if((semid = semget(key, 1, IPC_CREAT | 0600)) == -1)
    {
        errx(1, "Cannot create a semaphore with key = %d", key);
    }

    ctl.val = 1;
    if(semctl(semid, 0, SETVAL, ctl) == -1)
    {
        errx(2, "Error while rising the semaphore");
    }

    return semid;
}

void createQueue()
{
    remove(FIFO);
    umask(0);
    mkfifo(FIFO, 0666);
}

FILE *openFifo(char *parm)
{
    return fopen(FIFO, parm);
}

static void childSigHandler(int sig)
{
    switch(sig)
    {
        case SIGINT:
            fprintf(stdout, "Sprzatam...\n");
            fflush(stdout);
            munmap(suspendProcesses, sizeof *suspendProcesses);
            remove(FIFO);
            shmdt(memory);
            semctl(semid1, IPC_RMID, 0);
            semctl(semid2, IPC_RMID, 0);
            semctl(semid3, IPC_RMID, 0);

            for(int i = 0; i < 3; i++)
            {
                kill(childPIDS[i], SIGKILL);
            }
            kill(parentID, SIGKILL);
            break;

        case SIGUSR1:
            fprintf(stdout, "Suspending proces!\n");
            fflush(stdout);
            memset(suspendProcesses, 1, 1);
            break;

        case SIGUSR2:
            fprintf(stdout, "Resuming proces!\n");
            fflush(stdout);
            memset(suspendProcesses, 0, 1);
            break;

        default:
            break;
    }
}

static void declareSharedBool()
{
    suspendProcesses = mmap(NULL, sizeof *suspendProcesses, PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    memset(suspendProcesses, 0, 1);
}

void initSignals()
{
    // Shared bool to suspend processes
    declareSharedBool();

    // Ignoring all signals but defined 3
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigdelset(&mask, SIGINT);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    // Define signal action
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = &childSigHandler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
}

static void openInputFile(char *inputPath)
{
    inputFile = fopen(inputPath, "r");
    if(inputFile == NULL)
    {
        fprintf(stdout, "File path is incorrect!\n");
        fflush(stdout);
        exit(1);
    }
}

void parseArgs(int argc, char **argv)
{
    if(argc == 1)
    {
        runMode = 1;
    }
    else if(argc == 2)
    {
        if(strcmp(argv[1], "-u") == 0)
        {
            runMode = 3;
            openInputFile("/dev/urandom");
            lineBuffer = malloc(MSG_SIZE);
        }
        if(strcmp(argv[1], "-f") == 0)
        {
            fprintf(stdout, "Wrong parameter!\nUsage -f 'filepath'");
            exit(1);
        }
        if(strcmp(argv[1], "-h") == 0)
        {
            fprintf(stdout, "Usage:\n./project (interactive)\n./project -f filepath (from file)\n./project -u (from urandom)");
            exit(1);
        }
    }
    else if(argc == 3)
    {
        if(strcmp(argv[1], "-f") == 0)
        {
            runMode = 2;
            openInputFile(argv[2]);
            lineBuffer = malloc(MSG_SIZE);
        }
    }
}
