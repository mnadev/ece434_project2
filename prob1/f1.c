#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PROCESSMAX 10 //max process size is 10
#define CHILDRENMAX 3 //max children allowed is 3

// Structures to define the process tree
struct processInfo
{
    char processName; //A, B, C ... one char ONLY
    int numChild;
    char children[CHILDRENMAX];//
};

//Define the processes
struct processInfo process[PROCESSMAX];

//Process name
char pName;

//Recursive function
void pfunction()
{
    int i, j;
    int n = PROCESSMAX;
    printf("Started process %c, pid=%d\n", pName, getpid());
    
    //Get the children details
    for(i = 0; i < n; i++)
    {
        if(pName == process[i].processName) /* process details found */
            break;
    }
    if(i < n) //process info exists
    {
        //fork children
        pid_t pids[CHILDRENMAX];
        //++ error handle for more child than allowed
        for(j = 0; j < process[i].numChild; j++)
        {
            pids[j] = fork();
            if(pids[j] < 0)
            {
                printf("Process %c, pid=%d: fork failed\n", pName, getpid());
            }
            if(pids[j] == 0)
            {   //child process
                //update process name
                pName = process[i].children[j];
                //call the processfunction, and return
                pfunction();
                return;
            }
            else
            {   //parent process:
                printf("Process %c, pid=%d: Forked %c, pid=%d\n", pName, getpid(), process[i].children[j], pids[j]);
            }
        }
        //Children forked, wait for children to end
        printf("Process %c, pid=%d", pName, getpid());
        //: Waiting for children to end
        if(process[i].numChild>0){
            printf(": Waiting for children to end \n");
        }else{
            printf(": has no child process, sleep for sometime \n");
        }
        
        
        for(j = 0; j < process[i].numChild; j++)
        {
            int status;
            if(pids[j] > 0) //child forked successfully
            {
                waitpid(pids[j], &status, 0);
                printf("Process %c(pid=%d)'s child process %c with pid=%d has exited\n", pName,getpid(), process[i].children[j], pids[j]);
            }
        }
    }
    
    //Sleep for sometime
    sleep(2);
    printf("Process %c, pid=%d: ending process\n", pName, getpid());
}

int main(int argc, char* argv[])
{
    if(argc < 1){
        printf("error!\n");
        return 0;
    }
    //input file: read file and input info to array of struct "process"
    //++error handle -- 10 processes max
    
    process[0].processName='A'; // A 2 B C
    process[0].numChild=2;
    process[0].children[0]='B';
    process[0].children[1]='C';

    process[1].processName='B'; // B 1 D
    process[1].numChild=1;
    process[1].children[0]='D';

    
    process[2].processName='C'; // C 0
    process[2].numChild=0;
    
    process[3].processName='D'; // D 0
    process[3].numChild=0;
    
    //the process function:
    pName = process[0].processName;
    pfunction();
    return 0;
}
