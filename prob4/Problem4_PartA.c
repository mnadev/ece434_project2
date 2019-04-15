#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

//Global variables written to in signal handler:
int recvSum1;
int recvSum2;
int one_or_two;	//Set to 1 to send recvVal to Sum1 and Set to 2 to send recvVal to Sum2.
int recvMin;
int recvMax;
int canSignal; //Needed to prevent signal from being lost because another process already queued signal.
int child1HasSignaled;
int pid_Child2;

/* Signum = SIGUSR1 means that either a min value is being sent back to parent.
	Signum = SIGUSR2 means that a max value is being sent back to parent.
	Signum = SIGPIPE means that a partial sum is being sent back to the parent.
			(Using SIGPIPE because there's no way SIGPIPE will occur normally given that pipes are not used)
	Signum = SIGBUS means that the process can signal back to the parent again 
			(Using SIGBUS because I've never encountered it before so the program likely won't use it)*/
void sig_handler(int signum, siginfo_t* info, void* junk){
	int recvVal;
	switch(signum){
		case SIGUSR1:
			recvVal = info->si_value.sival_int;
			if(recvMin > recvVal){
				recvMin = recvVal;
			}
			break;
		case SIGUSR2:
			recvVal = info->si_value.sival_int;
			if(recvMax < recvVal){
				recvMax = recvVal;
			}
			break;
		case SIGPIPE:
			recvVal = info->si_value.sival_int;
			if(one_or_two == 1){
				recvSum1 = recvVal;
				one_or_two = 2;
				if(pid_Child2){
					kill(pid_Child2, SIGBUS);
				} else{
					child1HasSignaled = 1;
				}
			} else if(one_or_two == 2){
				recvSum2 = recvVal;
			}
			break;
		case SIGBUS:
			canSignal = 1;
			break;
	}
}


//Program will take one input parameter: The file to be read.
int main(int argc, char* argv[]){
	
	//Checking program parameters and if file exists.
	if(argc != 2){
		printf("Program usage: ./Prob4PtA <FileToBeRead>\n");
		exit(-1);
	}
	
	int inputFile = open(argv[1], O_RDONLY);	//File descriptor for file containing numbers
	if(inputFile < 0){
		printf("Fatal Error: Issue opening file '%s'\n", argv[1]);
		exit(-1);
	}
	
	//Initializing global values;
	//recvSum = 0;
	recvMin = 32767;
	recvMax = -1;
	canSignal = 1;
	child1HasSignaled = 0;
	pid_Child2 = 0;
	
	//Installing signal handler:
	sigset_t mask;	//Setting up mask to block signals when in signal handler
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);
	sigaddset(&mask, SIGPIPE);
	sigaddset(&mask, SIGBUS);
	//sigaddset(&mask, SIGALRM); For use in PartB
	
	//Setting up a mask for blocking purposes.
	sigset_t maskSIGBUS;
	sigset_t oldmask;
	sigemptyset(&maskSIGBUS);
	sigaddset(&maskSIGBUS,SIGBUS);
	
	struct sigaction sigSetupArgs;	//Setting up sigaction struct
	memset(&sigSetupArgs, 0, sizeof(struct sigaction));
	sigSetupArgs.sa_flags = SA_SIGINFO|SA_RESTART;		//SA_RESTART is needed so wait() doesn't return after signal handled.
	sigSetupArgs.sa_sigaction = sig_handler;
	sigSetupArgs.sa_mask = mask;
	
	sigaction(SIGUSR1, &sigSetupArgs, NULL);
	sigaction(SIGUSR2, &sigSetupArgs, NULL);
	sigaction(SIGPIPE, &sigSetupArgs, NULL);
	sigaction(SIGBUS, &sigSetupArgs, NULL);
	//sigaction(SIGALRM, &sigSetupArgs, NULL); //For use in PartB
	
	int* numArray = (int*)malloc(sizeof(int)*10);
	int arraySize = 10;
	int arrayIndex = 0;
	char charIn = 0;
	char readBuf[6] = {0,0,0,0,0,0};	//Integers are at most 5 characters long (32767 for signed, 65535 for unsigned) + \0 terminator
	int readBufIndex = 0;

	int eofCheck;
	int inputInt;
	while((eofCheck = read(inputFile, &charIn, 1))!=0){
		switch(charIn){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':	
				if(readBufIndex > 4){
					printf("Error: Input file contained a non-integer number. Terminating Program\n");
					exit(-1);
				}
				readBuf[readBufIndex] = charIn;
				readBufIndex++;
				break;
			case '\n':
				inputInt = atoi(readBuf);
				if(arrayIndex >= arraySize){
					arraySize *= 10;
					numArray = realloc(numArray, sizeof(int)*arraySize);
				}
				numArray[arrayIndex] = inputInt;
				arrayIndex++;
				memset(readBuf, 0, 6);
				readBufIndex = 0;
				break;
			default:
				printf("Input File Error: File contains non-digit characters. Terminating program\n");
				exit(-1);
		}
	}
	close(inputFile);
	arrayIndex--; //Fixing a segfault without having to refactor all the remaining code;
	numArray = realloc(numArray, sizeof(int)*(arrayIndex+1));
	//Shared memory removed, instead forking the entire array to all children
	
	//Output file to write results to
	FILE* output = fopen("Prob4PtA_Output.txt", "w");
	fprintf(output, "Hi I'm process %d and my parent is %d\n", getpid(), getppid());
	fflush(output);
	
	//Process creation
	int origPID = getpid();
	int left = 0;
	int right = arrayIndex;
	
	while(right-left > 1){
		one_or_two = 1;
		recvSum1 = 0;
		recvSum2 = 0;
		//Removed pipe initialization and RetVals struct
		sigprocmask(SIG_BLOCK, &maskSIGBUS, &oldmask);	//Blocking SIGBUS until later to fix a race condition.
		int pid1 = fork();
		if(pid1 == 0){
			sigprocmask(SIG_UNBLOCK, &maskSIGBUS, &oldmask);
			canSignal = 1;
			right = (right+left)/2;
			continue;
		}

		//Fork right half
		//Removed more pipe initialization
		int old_canSignal = canSignal;
		canSignal = 0;

		pid_Child2 = fork();
		if(pid_Child2 == 0){
			left = (right+left)/2 + 1;
			sigprocmask(SIG_UNBLOCK, &maskSIGBUS, &oldmask);
			continue;
		} else{
			canSignal = old_canSignal;
		}
		sigprocmask(SIG_UNBLOCK, &maskSIGBUS, &oldmask);
		if(getpid() != origPID){
			//Removed all pipe and struct related code, changed to support signals.
			wait(pid1);
			kill(pid_Child2, SIGBUS);
			wait(pid_Child2);
			
			sigprocmask(SIG_BLOCK, &maskSIGBUS, &oldmask);
			while(canSignal == 0){	//Don't want to signal until canSignal is true.
				sigsuspend(&oldmask);
			}
			sigprocmask(SIG_UNBLOCK, &maskSIGBUS, NULL);
			
			//Preparing to send information back to parent.
			union sigval retInfo = {recvMin};
			int parentPID = getppid();
			sigqueue(parentPID, SIGUSR1, retInfo);
			
			retInfo.sival_int = recvMax;
			sigqueue(parentPID, SIGUSR2, retInfo);
			
			retInfo.sival_int = recvSum1+recvSum2;
			sigqueue(parentPID, SIGPIPE, retInfo);	
			
			fprintf(output, "Hi I'm process %d and my parent is %d\n", getpid(), getppid());
			fflush(output);
			free(numArray);
			exit(1);
		} else{
			wait(pid1);
			kill(pid_Child2, SIGBUS);
			wait(pid_Child2);
			break;
		}
	}
	if(right - left <= 1){
		int tempMin = 32767;
		int tempMax = -1;
		int tempSum = 0;
		int i;
		for(i = left; i <= right; i++){
			tempMin = (tempMin < numArray[i]) ? tempMin : numArray[i];
			tempMax = (tempMax > numArray[i]) ? tempMax : numArray[i];
			tempSum += numArray[i];
		}
		
		sigprocmask(SIG_BLOCK, &maskSIGBUS, &oldmask);
		while(canSignal == 0){	//Don't want to signal until canSignal is true.
			sigsuspend(&oldmask);
		}
		sigprocmask(SIG_UNBLOCK, &maskSIGBUS, NULL);
		
		//Send info back to parent
		union sigval retInfo = {tempMin};
		int parentPID = getppid();
		sigqueue(parentPID, SIGUSR1, retInfo);

		retInfo.sival_int = tempMax;
		sigqueue(parentPID, SIGUSR2, retInfo);
		
		retInfo.sival_int = tempSum;
		sigqueue(parentPID, SIGPIPE, retInfo);
				
		fprintf(output, "Hi I'm process %d and my parent is %d\n", getpid(), getppid());
		fflush(output);
		free(numArray);
		exit(1);
	}
	
	fprintf(output, "Max = %d\nMin = %d\nSum = %d\n", recvMax, recvMin, recvSum1+recvSum2);
	fflush(output);
	free(numArray);
	fclose(output);
	return 0;
	
}	
