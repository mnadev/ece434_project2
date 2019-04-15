#ifndef PROB3HEADER
#define PROB3HEADER

#define STACKCAPACITY 100

//Defining Structs needed:
typedef struct _Tree_node{
    char* processName;
    int numChild;
    struct _Tree_node** children;
} Tree_node;

typedef struct _Stack{
	int indexOfTop;
	char stackContents[STACKCAPACITY];
} Stack;

//Defining function prototypes for Stack-related functions:
void stack_push(Stack* stack, char data);
char stack_pop(Stack* stack);
char stack_peek(Stack* stack);
int operatorValue(char op);

//Defining all the other function prototypes:
void evaluateTree(Tree_node* rootNode)
Tree_node* getTreeFromExpr(char* postfix);
void getTreeFromExprRec(char** postfixArray, Tree_node* currentNode);
char * readline(int fp);
char* infixToPostfix(char* infixExpression);
void print_tree(Tree_node* root, int level);


#endif