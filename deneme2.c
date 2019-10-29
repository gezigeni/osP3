// master.c
#include <stdio.h>
#include "shmemsem.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>

SharedData *shmem;
int shmid;

int main(int argc, char * argv[]) {

	srand ( time(NULL) );
	int randomTerminationNanosec = rand() % 1000000;
	
	printf("randomTerminationNanosec is %d\n", randomTerminationNanosec);
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
// Now initiate semaphore
	sem_init(&(shmem->mutex), 1, 1);


	
/*
	pid_t childpid = fork();
	if (childpid == 0) {
// Fork and exec off a child
		char *arglist[]={"./deneme2.c",NULL};
		execvp(arglist[0],arglist);
		perror("Exec failed");
	}
*/	

	int pid = getpid();


	int i = 0;
	while (i++ < 10) {
		printf("timeNanosec in CHILD = %d\n", shmem->timeNanosec);
		if(shmem->timeNanosec >= randomTerminationNanosec){
               	 	printf("Killing the process %d , at time %d", pid, shmem->timeNanosec);
                	kill(pid, SIGINT);
        	}
		sleep(rand() % 3);
// critical section
		sem_wait(&(shmem->mutex));
		printf("DENEME2 Entering critical section of process %d\n",getpid());
		sleep(rand() % 3);
		printf("DENEME2 Leaving critical section of process %d\n",getpid());
		sem_post(&(shmem->mutex));
	}
	wait();

// Detach
	shmdt(shmem);
// Free up shared memory 
	shmctl(shmid, IPC_RMID,NULL);

	return 0;
}
//
