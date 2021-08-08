#ifndef BINARY_SEMS_H
#define BINARY_SEMS_H


typedef enum{
    False = 0,
    True
}Boolean;

extern Boolean bsUseSemUndo;
extern Boolean bsRetryInEintr;

int initSemAvaiable(int semId,int semNum);

int initSemInUse(int semId,int semNum);

int reserveSem(int semId,int semNum);

int releaseSem(int semId,int semNum);


#endif //@ BINARY_SEMS_H