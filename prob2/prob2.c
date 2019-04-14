#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <pthread.h>
#include <signal.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PROCESSMAX 10 //max process size is 10
#define CHILDRENMAX 3 //max children allowed is 3

// Structures to define the process tree
typedef struct _processInfo
{
    char processName; //A, B, C ... one char ONLY
    int numChild;
    char children[CHILDRENMAX];//
} processInfo;

//Define the processes
processInfo process[PROCESSMAX];

int numProcesses = 0;

char pName;

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

void wait_for_children(int i, pid_t pids[]) {
    int j;
    for(j = 0; j < process[i].numChild; j++) {

            int status;

            if(pids[j] > 0) { //child forked successfully

                waitpid(pids[j], &status, 0);
                printf("Process %c(pid=%d)'s child process %c with pid=%d has exited\n", pName,getpid(), process[i].children[j], pids[j]);
                raise(SIGSTOP);
            }
    }
}

// Function to print a message after receiving SIGCONT
void print_message(int signum) {
    printf("I am process %d and I have received a SIGCONT", getpid());
}


// This is the function that is called when we create a process.
void process_handler(){
    int i, j;

    printf("Started process %c, pid=%d\n", pName, getpid());

    //Get the children details
    for(i = 0; i < numProcesses; i++) {
        if(pName == process[i].processName) /* process details found */
            break;
    }

    if(i < numProcesses) //process info exists
    {
        //fork children
        pid_t pids[process[i].numChild];
        //++ error handle for more child than allowed

        for(j = 0; j < process[i].numChild; j++) {

            pids[j] = fork();

            if(pids[j] < 0) {

                printf("Process %c, pid=%d: fork failed\n", pName, getpid());

            }

            if(pids[j] == 0) {   //child process

                //update process name
                pName = process[i].children[j];

                //call the processfunction, and return
                pfunction();
                return;

            } else {   //parent process:

                printf("Process %c, pid=%d: Forked %c, pid=%d\n", pName, getpid(), process[i].children[j], pids[j]);

            }
        }

        //Children forked, wait for children to end
        printf("Process %c, pid=%d", pName, getpid());

        //: Waiting for children to end
        if(process[i].numChild>0){

            printf(": Waiting for children to end \n");

        } else {

            printf(": has no child process, sleep for sometime \n");

        }
        
        wait_for_children(i, pids);        

    }
    
    
    // Send SIGCONT instead of sleeping
    kill(getppid(), SIGCONT);
    printf("Process %c, pid=%d: ending process\n", pName, getpid());
}

// Function to read a line
void readline(FILE* fp){

    // Malloc a char array. 250 chars should be sufficient
    char * line = (char *) malloc(sizeof(char) * 250);

    // i points to where we are in the char array
    int i = 0;

    // Keep reading a char until we hit eof or a new line
    while(fgets(line[i], 1, fp) != '\n' || !feof(fp)) {

        if(i == 249) {
            break;
        }

        i++;
    }

    // set null terminator
    line[i] = '\0';
    return line;
}

int main(int argc, char* argv[])
{
    if(argc < 2){
        printf("error!\n");
        return 0;
    }
    //input file: read file and input info to array of struct "process"
    //++error handle -- 10 processes max

    // Assume file is passed in as the second command line argument
    char * filename = argv[1];

    FILE *fp;

    fp = fopen("filename", "r+");

    // Keep reading a line until eof. 
    // After each line, parse it and generate any nodes.

    int lineNum = 0;

    while (!feof(fp)) {
        const char *line = readLine(fp);
        parseLine(line, lineNum);

        lineNum++;
    }

    numProcesses = lineNum;

    // set current node to head and call process handler
    pName = process[0].processName;
    process_handler();

    fclose(fp);

    return 0;
}