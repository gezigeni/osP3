#ifndef SHMEMSEM_H
#define SHMEMSEM_H

#include <semaphore.h>
#include "queue.h"
#include <stdio.h>
#define BUFSIZE 64


typedef struct {
	int shmInt;
	int timeSec;
	int timeNanosec;
	int shmMsgSec;
	int shmMsgNanosec;
	pid_t pid;
	sem_t mutex;
	struct Queue pid_queue;
	struct Queue time_queue;
} SharedData;

#endif
