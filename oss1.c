// master.c
#include <stdio.h>
#include <unistd.h>
#include "shmemsem.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFSIZE 64

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
	int totalProcessDied = 0;
	
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
//----------------------------------------------------------------

// Now initiate semaphore
	sem_init(&(shmem->mutex), 1, 1);
	shmem->timeSec = 0;
	shmem->timeNanosec = 0;
	shmem->counter = 0;
	

	//int pid_arr[numberOfProcess]
	printf("THIS IS OSS PID = %ld\n", getpid());
	int masterPid = getpid();
printf("number of process = %d\n", numberOfProcess);	
       for (j = 0; j < numberOfProcess; j++){
       		if (childpid = fork())
         		break;
		shmem->pid_arr[j] = getpid();
printf("pid_arr = %d\n", shmem->pid_arr[j]);
	}

	if (getpid() != masterPid) {
		char *arglist[]={"./user",NULL};
		execvp(arglist[0],arglist);
		perror("Exec failed");
	}
	printf("EXECI GECEN PID = %ld\n", getpid());
	int i = 0;
	while (1) {

	
		sem_wait(&(shmem->mutex));
		shmem->timeNanosec += 50;
		if(shmem->timeNanosec >= 1000000000){
			shmem->timeNanosec -= 1000000000;
			shmem->timeSec++;
		}
		sem_post(&(shmem->mutex));
		while(shmem->counter > 0){
			totalProcessDied++;
			sem_wait(&(shmem->mutex));	
			printf("OSS Entering critical section of process %d\n",getpid());
			
					
			fprintf(fp, "Child pid %ld is terminating at my time %d second, %d nanosecond because it reached %d second, %d nanosecond in child process\n", (long)shmem->pid, shmem->timeSec, shmem->timeNanosec, shmem->shmMsgSec, shmem->shmMsgNanosec);		
			printf("Child pid %ld is terminating at my time %d second, %d nanosecond because it reached %d second, %d nanosecond in child process\n", (long)shmem->pid, shmem->timeSec, shmem->timeNanosec, shmem->shmMsgSec, shmem->shmMsgNanosec); 
			shmem->counter--;
			for(i = 0; i < numberOfProcess; i++){
				if(shmem->pid_arr[i] == shmem->pid){
					childpid = fork();
					if(childpid == 0){
						shmem->pid_arr[i] = getpid();
						char *arglist[]={"./user",NULL};
						execvp(arglist[0],arglist);					
					}			
					break;					
				}
			}
			
                	printf("OSS Leaving critical section of process %d\n",getpid());
                	sem_post(&(shmem->mutex));


		}

		if(totalProcessDied > 10){
			shmdt(shmem);
			shmctl(shmid, IPC_RMID,NULL);
			fclose(fp);				
		}
	}
	wait();


}
//
