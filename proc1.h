//
// Created by kuba on 1/6/18.
//

#ifndef PROJEKTSO_PROC1_H
#define PROJEKTSO_PROC1_H

void startProc1(int semid1, int semid2, int semid3, void *memory);
void startProc1withFile(int semid1, int semid2, int semid3, void *memory, char *infile);

#endif //PROJEKTSO_PROC1_H
