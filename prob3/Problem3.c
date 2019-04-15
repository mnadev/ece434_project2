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
#include <limits.h>
#include <ctype.h>
#include "Problem3_Header.h"

//Global variables:
int postfixIndex;

int evaluateTree(Tree_node* rootNode){
	//Process creation
	if(isdigit(rootNode->processName[0])){
		return atoi(rootNode->processName);
	}
	
	int pipeLeft[2];
	pipe(pipeLeft);
	int pidLeft = fork();
	int leftResult;
	if(pidLeft){
		close(pipeLeft[1]);
	} else{
		close(pipeLeft[0]);
		int retVal = evaluateTree(rootNode->children[0]);
		write(pipeLeft[1],&retVal,sizeof(int));
		exit(0);
	}

	int pipeRight[2];
	pipe(pipeRight);
	int pidRight = fork();
	int rightResult;
	if(pidRight){
		close(pipeRight[1]);
	} else{
		close(pipeRight[0]);
		int retVal = evaluateTree(rootNode->children[1]);
		write(pipeRight[1], &retVal, sizeof(int));
		exit(0);
	}
	
	read(pipeLeft[0], &leftResult, sizeof(int));
	read(pipeRight[0], &rightResult, sizeof(int));
	waitpid(pidLeft, NULL, 0);
	waitpid(pidRight, NULL, 0);
	int finalResult = 0;
	switch(rootNode->processName[0]){
		case '+':
			finalResult = leftResult+rightResult;
			break;
		case '*':
		case 'x':
			finalResult = leftResult*rightResult;
			break;
	}
	return finalResult;
}

Tree_node* getTreeFromExpr(char* postfix){
	//First splitting up the postfix expression into its tokens and placing them into an array:
	int numTokens = 0;
	char** tokenArray = malloc(sizeof(char*)*150);
	memset(tokenArray, 0, sizeof(char*)*150);
	
	char* token = strtok(postfix, " ");
	while(token != NULL){
		tokenArray[numTokens] = token;
		numTokens++;
		token = strtok(NULL, " ");
	}
	numTokens--;	//Because numTokens will increment once more before strtok returns NULL
	postfixIndex = numTokens;
	Tree_node* rootNode = malloc(sizeof(Tree_node));
	rootNode->processName = tokenArray[postfixIndex];
	rootNode->numChild = 2;
	postfixIndex--;
	
	rootNode->children = malloc(sizeof(Tree_node)*2);
	Tree_node* leftChildNode = malloc(sizeof(Tree_node));
	rootNode->children[0] = leftChildNode;
	getTreeFromExprRec(tokenArray, leftChildNode);
	
	Tree_node* rightChildNode = malloc(sizeof(Tree_node));
	rootNode->children[1] = rightChildNode;
	getTreeFromExprRec(tokenArray, rightChildNode);
	
	return rootNode;
}

void getTreeFromExprRec(char** postfixArray, Tree_node* currentNode){
	currentNode->processName = postfixArray[postfixIndex];
	postfixIndex--;
	if(currentNode->processName[0] == '+' || currentNode->processName[0] == '*' || currentNode->processName[0] == 'x'){
		currentNode->numChild = 2;
	} else{
		currentNode->numChild = 0;
	}
	if(currentNode->numChild == 0){		//basecase
		return;
	} else{
		currentNode->children = malloc(sizeof(Tree_node)*2);
		
		Tree_node* leftChildNode = malloc(sizeof(Tree_node));
		currentNode->children[0] = leftChildNode;
		getTreeFromExprRec(postfixArray, leftChildNode);
		
		Tree_node* rightChildNode = malloc(sizeof(Tree_node));
		currentNode->children[1] = rightChildNode;
		getTreeFromExprRec(postfixArray, rightChildNode);
	}
}

// Function to read a line
char * readline(int fp){
	
	int eof = 0;
    // Malloc a char array. 250 chars should be sufficient
    char * line = (char *) malloc(sizeof(char) * 250);
	memset(line, 0, sizeof(char)*250);

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

char* infixToPostfix(char* infixExpression){
	//We assume that the expression we're evaluating contains no negative numbers.
	//Initializing stack data structure:
	Stack* operators = malloc(sizeof(Stack));
	memset(operators, 0, sizeof(Stack));
	operators->indexOfTop = -1;
	
	char* postfixExpr = malloc(sizeof(char)*1000);
	memset(postfixExpr, 0, sizeof(char)*1000);

	char readBuf[7] = {0,0,0,0,0,0,0};	//buffer for integers.
	int readBufIndex = 0;

	int readinLength = 0;
	while(infixExpression[readinLength] != '\0'){
		switch(infixExpression[readinLength]){
			case '\n':
			case ' ':
				if(readBufIndex != 0){
					readBuf[readBufIndex] = ' ';
					strcat(postfixExpr, readBuf);
					readBufIndex = 0;
					memset(readBuf, 0, 7);
				}
				break;
			case '(':
			case '[':
				if(readBufIndex != 0){
					readBuf[readBufIndex] = ' ';
					strcat(postfixExpr, readBuf);
					readBufIndex = 0;
					memset(readBuf, 0, 7);
				}			
				stack_push(operators, '(');
				break;
			case ')':
			case ']':
				if(readBufIndex != 0){
					readBuf[readBufIndex] = ' ';
					strcat(postfixExpr, readBuf);
					readBufIndex = 0;
					memset(readBuf, 0, 7);
				}			
				while(operators->indexOfTop != -1 && stack_peek(operators) != '('){
					char addToExpr[3] = {stack_pop(operators), ' ', '\0'};
					strcat(postfixExpr, addToExpr);
				}
				if(operators->indexOfTop != -1 && stack_peek(operators) != '('){
					printf("Fatal Error: Incorrectly formatted input expression\n");
					exit(-1);
				}
				stack_pop(operators);
				break;
			case '+':
			case '*':
			case 'x':
				if(readBufIndex != 0){
					readBuf[readBufIndex] = ' ';
					strcat(postfixExpr, readBuf);
					readBufIndex = 0;
					memset(readBuf, 0, 7);
				}			
				while(operators->indexOfTop != -1 && (operatorValue(infixExpression[readinLength]) <= operatorValue(stack_peek(operators)))){
					char addToExpr[3] = {stack_pop(operators), ' ', '\0'};
					strcat(postfixExpr, addToExpr);
				}
				stack_push(operators, infixExpression[readinLength]);
				break;
			default:
				readBuf[readBufIndex] = infixExpression[readinLength];
				readBufIndex++;
				break;
		}
		readinLength++;
	}
	
	if(readBufIndex != 0){
		readBuf[readBufIndex] = ' ';
		strcat(postfixExpr, readBuf);
	}				
	while(operators->indexOfTop != -1){
		char addToExpr[3] = {stack_pop(operators), ' ', '\0'};
		strcat(postfixExpr, addToExpr);
	}
	
	free(operators);
	postfixExpr = realloc(postfixExpr, sizeof(char)*(strlen(postfixExpr)+1));
	return postfixExpr;
}


int main(int argc, char* argv[])
{
    if(argc < 2){
        printf("Fatal Error! Program Usage: ./Prob3 <InputExpressionFile>\n");
        return -1;
    }

    // Assume file is passed in as the second command line argument
    char * filename = argv[1];

    int inputFile;
    inputFile = open(filename,  O_RDONLY);  
	if(inputFile < 0){
		printf("Fatal Error: Error opening file %s\n", filename);
	}

    //Reading input expression:
	char* inputExpression = readline(inputFile);
    close(inputFile);
	
	char* postfixExpr = infixToPostfix(inputExpression);
	free(inputExpression);
	//printf("%s\n", postfixExpr);		//Debug statement ensuring correctness of postfix.
	
	Tree_node* rootNode = getTreeFromExpr(postfixExpr);
	//print_tree(rootNode, 0);			//Debug statement ensuring correctness of tree structure.

	int result = evaluateTree(rootNode);
	printf("The result of the expression is %d\n", result);

    return 0;
}

void stack_push(Stack* stack, char data){
	stack->indexOfTop++;
	stack->stackContents[stack->indexOfTop] = data;
}
char stack_pop(Stack* stack){
	if(stack->indexOfTop == -1){
		return 0;
	}
	char poppedValue = stack->stackContents[stack->indexOfTop];
	stack->indexOfTop--;
	return poppedValue;
}
char stack_peek(Stack* stack){
	return stack->stackContents[stack->indexOfTop];
}
int operatorValue(char op){
	if(op == '+'){
		return 1;
	} else if(op == '*' || op == 'x'){
		return 2;
	} else{
		return -1;
	}
}

void print_tree(Tree_node* root, int level){
	int i;
	for(i = 0; i < level; i++){
		printf("\t");
	}
	printf("%s\n", root->processName);
	for(i = 0; i < root->numChild; i++){
		print_tree(root->children[i], level+1);
	}
}