// child.c
#include <stdio.h>
#include "shmemsem.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

SharedData *shmem;
int shmid;

bool isGreater(int s1, int s2, int n1, int n2){
	if(s1>s2)
		return true;
	else if(s1<s2)
		return false;
	else{
		if(n1>n2)
			return true;
		else
			return false;
	}
}

int main(int argc, char * argv[]) {
	time_t t;
	srand((int)time(&t) % getpid());
	int randomTerminationNanosec = rand() % 1000000;

// Create our key
	key_t shmkey = ftok("test.txt",777);
	if (shmkey == -1) {
		perror("Ftok failed");
		exit(-1);
	}
// Get our shm id
	shmid = shmget(shmkey, sizeof(SharedData), 0600);
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
//------------------------------------------------------------

// Now initiate semaphore
	sem_init(&(shmem->mutex), 1, 1);

	int startingSec = shmem->timeSec;
        int startingNanosec = shmem->timeNanosec;
        int finishingSec = shmem->timeSec;
        int finishingNanosec = randomTerminationNanosec + startingNanosec;

	 if(finishingNanosec > 1000000000){
                finishingNanosec -= 1000000000;
                finishingSec++;
        }

	int pid = getpid();

	int i = 0;
	while (1) {
	
		if(isGreater(shmem->timeSec, finishingSec, shmem->timeNanosec, finishingNanosec) && shmem->counter == 0){
			
			sem_wait(&(shmem->mutex));
                	printf("USER Entering critical section of process %d\n",getpid());	

	
						
			shmem->shmMsgSec = shmem->timeSec;
			shmem->shmMsgNanosec = shmem->timeNanosec;
			shmem->pid = getpid();
printf("USER counter = %d before increment\n ", shmem->counter);			
			shmem->counter++;					
printf("USER counter = %d after increment\n ", shmem->counter);
                	printf("USER Leaving critical section of process %d\n",getpid());
                	sem_post(&(shmem->mutex));		

                   	//shmdt(shmem);
			//shmctl(shmid, IPC_RMID,NULL);
		        exit(1);
                }
	}
	return 0;
}
//
