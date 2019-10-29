// master.c
#include <stdio.h>
#include "shmemsem.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

SharedData *shmem;
int shmid;

int main() {
// Create our key
	key_t shmkey = ftok("test.txt",777);
	if (shmkey == -1) {
		perror("Ftok failed");
		exit(-1);
	}
// Get our shm id
	shmid = shmget(shmkey, sizeof(SharedData), 0600 | IPC_CREAT);
	if (shmid == -1) {
		perror("Failed to allocate shared memory region");
		exit(-1);
	}
// Attach to our shared memory
	shmem = (SharedData*)shmat(shmid, (void *)0, 0);
	if (shmem == (void *)-1) {
		perror("Failed to attach to shared memory region");
		exit(-1);
	}
// Now initiate semaphore
	sem_init(&(shmem->mutex), 1, 1);
	shmem->timeSec = 0;
	shmem->timeNanosec = 0;
	printf("timeSec is %d\n", shmem->timeSec);
	pid_t childpid = fork();
	if (childpid == 0) {
// Fork and exec off a child
		char *arglist[]={"./deneme2",NULL};
		execvp(arglist[0],arglist);
		perror("Exec failed");
	}
	int i = 0;
	while (i++ < 10) {
		shmem->timeNanosec += 100000;
		if(shmem->timeNanosec >= 1000000000){
			shmem->timeNanosec -= 1000000000;
			shmem->timeSec++;
		}
		printf("timeNanosec is %d\n", shmem->timeNanosec);
		printf("timeSec is %d\n", shmem->timeSec);
		sleep(rand() % 3);
// critical section
		sem_wait(&(shmem->mutex));
		printf("Entering critical section of process %d\n",getpid());
		sleep(rand() % 3);
		printf("Leaving critical section of process %d\n",getpid());
		sem_post(&(shmem->mutex));
	}
	wait();

// Detach
	shmdt(shmem);
// Free up shared memory 
	shmctl(shmid, IPC_RMID,NULL);
}
//
