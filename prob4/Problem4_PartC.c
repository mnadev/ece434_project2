#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <signal.h>
#include <fcntl.h> 
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h> 


#define PROCESSMAX 10 //max process size is 10
#define CHILDRENMAX 3 //max children allowed is 3

int * sigCnt;

void customSigHandler(int sig) 
{
    //sigCnt[sig-1] +=  1;
    printf("Caught signal %d; TID: %d\n", sig, (int)pthread_self());
    sleep(3);
} 

// Structures to define the process tree
typedef struct _processInfo
{
    char processName; //A, B, C ... one char ONLY
    int numChild;
    char children[CHILDRENMAX];//
} processInfo;

//Define the processes
processInfo process[PROCESSMAX];


int eof = 0;



//Recursive function
void* pfunction(void* curThreadNamePtr)
{
    char curThreadName = *((char*)curThreadNamePtr);
    int i, j;
    int n = PROCESSMAX;
    printf("Started thread %c, tid=%d\n", curThreadName, (int)pthread_self());

    //Get the children details
    for(i = 0; i < n; i++)
    {
        if(curThreadName == process[i].processName) /* process details found */
            break;
    }



    if(i < n) //process info exists
    {
        //fork children
        pthread_t tids[CHILDRENMAX];
        //++ error handle for more child than allowed
        for(j = 0; j < process[i].numChild; j++)
        {
            char * child = &(process[i].children[j]);
            pthread_create(&(tids[j]),NULL,pfunction,(void*)child);
            printf("Thread %c, tid=%d: Thread created %c, tid=%d\n", curThreadName, (int)pthread_self(), process[i].children[j], (int)tids[j]);
        }
        //: Waiting for children to end
        if(process[i].numChild>0){
            printf(": Waiting for children to end \n");
        }else{
            printf(": has no child thread, sleep for 5 seconds \n");
        }

        for(j = 0; j < process[i].numChild; j++)
        {
            int status;
            if(tids[j] > 0) //child forkewd successfully
            {
                pthread_join(tids[j], NULL);
                printf("Thread %c(tid=%d)'s child thread %c with tid=%d has exited\n", curThreadName,(int)pthread_self(), process[i].children[j], (int)tids[j]);
            }
        }
    }

    //Sleep for sometime
    sleep(5);
    printf("Thread %c, tid=%d: ending thread\n", curThreadName, (int)pthread_self());
}

void parseLine(char * line, int lineNum){
    char * tokenLine = (char *) malloc(sizeof(char) * 250);
    strcpy(tokenLine, line);

    // tokenize the line by spaces
    char* token = strtok(tokenLine, " ");

    int gotCurrentNode = 0;
    int gotNumChildren = 0;

    int childrenPtr = 0;

    // create the process info struct
    processInfo * proc = (processInfo *) malloc(sizeof(processInfo));

    // Parse the tokens
    while (token != NULL) {

        // get the current node if we haven't already
        // and store as the process Name
        if(gotCurrentNode == 0) {
            proc ->  processName = token[0];
            gotCurrentNode = 1;
            token = strtok(NULL, " ");
            continue;
        }

        // get the number of children
        // and store as process name
        if(gotNumChildren == 0) {
            proc -> numChild = atoi(token);
            gotNumChildren = 1;
            token = strtok(NULL, " ");
            continue;
        }

        // set the child into the array
        proc -> children[childrenPtr] = token[0];

        childrenPtr++;

        token = strtok(NULL, " ");
    }

    process[lineNum] = *proc;

    return;
}

// Function to read a line
char * readline(int fp){

    // Malloc a char array. 250 chars should be sufficient
    char * line = (char *) malloc(sizeof(char) * 250);

    // i points to where we are in the char array
    int i = 0;

    char charIn;
    do{
		int status = read(fp, &charIn, 1);
        if (status == 0) {
            eof = 1;
            line[i] = '\0';
            return line;
        }
		line[i] = charIn;
		i++;

    } while(charIn != '\n');

    // set null terminator
    line[i] = '\0';
    return line;
}


int main(int argc, char* argv[])
{
    //sigCnt = malloc(20*sizeof(int));
    //memset(sigCnt,0,sizeof(int)*20);
    // struct sigaction psa;
    // psa.sa_handler = customSigHandler;
    // //psa.sa_flags = SA_RESETHAND;
    // sigaddset(&psa.sa_mask, SIGABRT);
    // sigaddset(&psa.sa_mask, SIGSEGV);

    // sigaction(SIGINT, &psa, NULL);
    // //sigaction(SIGSEGV, &psa, NULL);
    // sigaction(SIGTERM, &psa, NULL);
    // sigaction(SIGQUIT, &psa, NULL);
    // sigaction(SIGTSTP, &psa, NULL);
    // sigaction(SIGABRT, &psa, NULL);

    if(argc < 2){
        printf("error!\n");
        return 0;
    }
    //input file: read file and input info to array of struct "process"
    //++error handle -- 10 processes max

    //input file: read file and input info to array of struct "process"
    //++error handle -- 10 processes max

    // Assume file is passed in as the second command line argument
    char * filename = argv[1];

    int fp;
    fp = open(filename,  O_RDWR);  

    // Keep reading a line until eof. 
    // After each line, parse it and generate any nodes.

    int lineNum = 0;

    while (eof == 0) {
        parseLine(readline(fp), lineNum);

        lineNum++;
    }
    
    close(fp);

    //the process function:
    char pName = process[0].processName;
    char * pNamePtr = &pName;
    printf("I am the main thread.  PID: %d\n",(int)getpid());
    pfunction((void*)pNamePtr);
    return 0;
}
