// master.c
#include <stdio.h>
#include <unistd.h>
#include "shmemsem.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//#define BUFSIZE 64

SharedData *shmem;
int shmid;

int main(int argc, char * argv[]) {
// Create our key
	int j; 
	pid_t childpid;
	char  outputfilename[BUFSIZE+1];
        char c;
	FILE *fp;
        int sflag = 0;
        int lflag = 0;
        int tflag = 0;
        int numberOfProcess = 5;

	
	while((c = getopt(argc, argv, "hs:l:t:")) != -1){
    
		switch(c){
        		case 'h':
            			printf("The program can take 4 command-line arguments such as;\n");
            			printf("-h which displays the legal command-line options \n");
            			printf("-s which takes another argument, inputfilename \"-i inputfilename\" as an example\n");
            			printf("-l which takes another argument, outputfilename \"-o outputfilename\" as an example\n");
            			printf("-t which takes another argument, time in seconds \"-t 15\" as an example\n");
            			break;
        		case 's':
            			sflag = 1;
            			break;
        		case 'l':
				snprintf(outputfilename, BUFSIZE, "%s", optarg );
            			printf("-l option: %s \n", outputfilename);
            			lflag = 1;
            			break;
        		case 't':
            			tflag = 1;
            			break;
        		default:
            			printf("Not a valid command-line argument");
            			break;
            
   		}
    
	}

	if(lflag){
                fp = fopen(outputfilename,"w");
                if (fp == NULL){
                        perror("Error while opening the file.\n");
                        exit(EXIT_FAILURE);
                }
        }
        else{
                fp = fopen("output.dat", "w");
                if (fp == NULL){
                        perror("Error while opening the file.\n");
                        exit(EXIT_FAILURE);
                }
        }


	key_t shmkey = ftok("Makefile",777);
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
	//printf("timeSec is %d\n", shmem->timeSec);

	for(j = 1; j <= numberOfProcess; j++){
                if ((childpid = fork()) <= 0){
                        break;
                }

        }



	//pid_t childpid = fork();
	if (childpid == 0) {
// Fork and exec off a child
		char *arglist[]={"./user",NULL};
		execvp(arglist[0],arglist);
		perror("Exec failed");
	}
	int i = 0;
	while (i++ < 10) {

		if(shmem->shmMsgNanosec > 0){
                	//printf("MESSAGE FROM CHILD TO PARENT IS SENT");
                	fprintf(fp, "Child pid %ld is terminating at my time %d second, %d nanosecond because it reached %d second, %d nanosecond in child process\n", (long)shmem->pid, shmem->timeSec, shmem->timeNanosec, shmem->timeSec, shmem->shmMsgNanosec);
			//fprintf(fp, " process ID:%ld", (long)getpid());
			//childpid = fork();
			shmem->shmMsgNanosec = 0;
        	}

		shmem->timeNanosec += 100000;
		if(shmem->timeNanosec >= 1000000000){
			shmem->timeNanosec -= 1000000000;
			shmem->timeSec++;
		}
		//printf("timeNanosec is %d\n", shmem->timeNanosec);
		//printf("timeSec is %d\n", shmem->timeSec);
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
	fclose(fp);
}
//
