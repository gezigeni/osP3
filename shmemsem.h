#ifndef SHMEMSEM_H
#define SHMEMSEM_H

#include <semaphore.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 64
#define NUM 5

static numberOfProcess = 5;

/*typedef struct {
    int front, rear, size;
    unsigned capacity;
    int *array;
} Queue;
*/

typedef struct {
	int shmInt;
	int timeSec;
	int timeNanosec;
	int shmMsgSec;
	int shmMsgNanosec;
	int counter;
	sem_t mutex;
	int pid;
	int pid_arr[NUM];
} SharedData;


#endif
