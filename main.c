#include <stdio.h>
#include <stdlib.h>

typedef struct AST_Node {
    int isNumberFLG; //
    // data
    int number;
    char operator;

    //tree variables
    struct AST_Node *right;
    struct AST_Node *left;
}AST_Node;
typedef struct stack {
    int size;
    int top;
    char *stackArr;
}stack;

                                                        // exception
int exceptionNum = 0;

void exceptionTEST(int type);
void panic( char* msg);


                                                        //Tokenizer
typedef enum {
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF,
    TOKEN_UNDEFINED
}tokenType;
typedef struct token {
    tokenType type;
    int value; // use only if type == TOKEN_NUMBER
    struct token* next;
}token ;
token *tokenize(char * input);
tokenType getToken(char character);

                                                        // Mem free
void freeLL(token * ll);

                                                        //supporting functions
int isNumber(char character);
int getNum(char * input, int * inputIDX);


                                                        //stacks
char *postFix(token * ll);
char getOperatorType(tokenType type);
int precedanceCheck(char curOperator,char topStackOperator);

void push(char operator, stack * st);
char  pop(stack * st);
char  peek(stack * st);

int main(void)
{
                                                         //get input and solution
    printf("Equation: 2 + 3 * (4 -1) \nSolve the equation using\n 1.Infix \n 2.AST_tree\n");
    char *input = "2 + 3 * (4 - 1)\0";                    // string to interpret
    int choose = 0;
    scanf("%d",&choose);
                                                        //get rid of spaces in input
                                                        //tokenized array & other set flag mem


    token *tokenLL = tokenize(input);

    char * postfix = postFix(tokenLL);

                                                        //free malloced memory
    freeLL(tokenLL);
    free(postfix);


    return 0;
}

// tokenizes input string as a linked list
token * tokenize(char * input) {
    token * head = malloc(sizeof(token));
    token *cur;
    cur = head;
                                                                    //NULL string case
    if (input == NULL) {
        head = NULL;
        return head;
    }

    //input string counter
    int i = 0;
    while(input[i] != '\0') {
        //number check

        if(isNumber(input[i])) {
            cur->type = TOKEN_NUMBER;
                                                                        // save number, account for numbers bigger than 1 digit
            cur->value = getNum(input,&i);

            cur-> next = malloc(sizeof(token));                      //reserve space for next node
            cur-> next->value = 0;
            cur-> next -> next = NULL;
            cur = cur->next;
        }
        //operator check
        else {
            cur->type = getToken(input[i]);
                                                                            //error handling
            if(cur->type == TOKEN_UNDEFINED) {
                exceptionTEST(1);
                return head;
            }
                                                                            //check to make sure '-' sign is not a negative sign for a number
                                                                            // if it is, call getNum() and continue to next token

            if(cur->type == TOKEN_MINUS && isNumber(input[i+1])) {
                cur-> type = TOKEN_NUMBER;
                i++;
                cur-> value = getNum(input, &i);
            }

                                                                            //reserve space for next node
            cur->next = malloc(sizeof(token));
            cur-> next->value = 0;
            cur-> next -> next = NULL;
            cur = cur->next;

        }
        i++;
    }
    return head;
}
tokenType getToken(char  character) {
    switch (character) {
        case 0x00:// NULL
            return TOKEN_EOF;
        case 0x2B:// +
            return TOKEN_PLUS;
        case 0x2D:// -
            return TOKEN_MINUS;
        case 0x2A:// *
            return TOKEN_MUL;
        case 0x78:// *
            return TOKEN_MUL;
        case 0x2F:// /
            return TOKEN_DIV;
        case 0x28:// (
            return  TOKEN_LPAREN;
        case 0x29:// )
            return TOKEN_RPAREN;
        default:
            return TOKEN_UNDEFINED;
    }

}

//Number Token Functions
int isNumber(char character) {
    if(character >= '0' && character <= '9' )
        return 1;
    return 0;
}
int getNum(char * input, int * inputIDX) {
    int *i = inputIDX;
    int tempNumArr[21];
    int tempNum = 0;

    if(isNumber(input[*i])) {
        int Digits = 0;
        while(isNumber(input[*i])) {
            tempNumArr[Digits] = input[*i] - '0';
            (*i)++;
            Digits++;
        }
        for(int j = Digits; j>=0;j--) {                       //create completed number using saved digits
            tempNum+= tempNumArr[Digits-j] * 10*j;            //gets correct 10 exponent while getting correct spot in arr(digits-j)
        }
    }
    else {
                                                             // error handling, if given input is not a number
        exceptionTEST(2);
    }
    return tempNum;
}

//Postfix Function
char * postFix(token *ll) {
    stack * opStack = malloc(sizeof(stack));
    token *temp = ll;
    char * postfixArr;                                          // arr to save completed postfix expression
    int size = -1;                                              // stack size tracker
    int postfixIDX = 0;                                             // for postfix arr
    char curOperator = 0;                                       // current operator to be put onto stack
    int curPrecedance = 0;                                      // used to determine realtion between cur operator and stack top operator
                                                                //get maximum size of stack for operators
    while(temp!=NULL) {
        size++;
        temp = temp->next;
    }
    free(temp);
    opStack->size = size;
    opStack->top = -1;
    postfixArr = malloc(sizeof(char)*size);                  //properly size result, and stack arrays
    opStack->stackArr = malloc(sizeof(char )*size);

    while(ll!=NULL) {                                           //while there are tokens to be read
                                                                //IF NUMBER
        if(ll->type ==TOKEN_NUMBER) {                           //TODO ll might point to dealocated memory?
            postfixArr[postfixIDX] = ll -> value;
            postfixIDX++;
        }
                                                                //IF OPERATOR
        else{
            //makes sure we don't overflow the stack
            if(opStack->top!=opStack->size) {

                curOperator = getOperatorType(ll->type);                     //grab current operator
                if(opStack->top == 0) {                                      // if stack is empty, PUSH current operator
                    push(curOperator, opStack);
                    postfixIDX++;
                }

                else {
                                                                            //this takes the current operator, and finds the top operator on the stack located at
                                                                            //operatorStack -> stackArr -> IDX[operatorStack.top]
                                                                            //check precedance of top stack operator to current operator
                    curPrecedance = precedanceCheck(curOperator, opStack->stackArr[opStack->top]);
                    switch(curPrecedance) {
                        case 2: //current operator is '(' or ')'
                            if(curOperator == ')') {                                 // close bracket logic
                                while(opStack->stackArr[opStack->top] != '(') {      // pop stack until end bracket reached
                                    postfixArr[postfixIDX] = pop(opStack);
                                    postfixIDX++;                                    // go to next index in postfix arr
                                }                                                    //TODO: error if this doesn't break from the loop
                                opStack->top--;                                      // remove ')' from stack
                            }
                            else {
                                //curOperator is a staring bracket
                                push(curOperator,opStack);
                            }
                            break;

                        case 1: //current operators is higher precedance to current top
                            push(curOperator, opStack);
                            break;

                        case 0://cur is lower precedance to top
                            //pop until end of stack, '(' is seen, or if precedence is the same as the new top
                            while(opStack->top != -1 || opStack->stackArr[opStack->top] != '(' || precedanceCheck(curOperator, opStack->stackArr[opStack->top] ) != -1) {
                                postfixArr[postfixIDX] = pop(opStack); //pop top operator to array
                                postfixIDX++;
                            }
                            push(curOperator, opStack); //push new operator
                            break;

                        case -1: //Same precedance                      //TODO will impliment ^ after all infix->postfix code is working
                            postfixArr[postfixIDX] = pop(opStack);      //POP old operator
                            postfixIDX++;
                            push(curOperator,opStack);                  //Push new operator
                            break;
                        default:// should never reach here
                            exceptionTEST(4);
                    } //end of switch case
                } //end of if-else empty stack push check
            } // overflow stack check
        } //if-else number/operator check
        ll = ll->next;
    }// end of given LL

    //pop everything left on the stack
    while (opStack->top != -1) {
        postfixArr[postfixIDX++] = pop(opStack);
    }



    free(opStack->stackArr);
    free(opStack);
    return postfixArr;
}
//support functions-------------
char getOperatorType(tokenType type) {
    if(type ==TOKEN_PLUS)
        return '+';
    if(type ==TOKEN_MINUS)
        return '-';
    if(type ==TOKEN_MUL)
        return '*';
    if(type ==TOKEN_DIV)
        return '/';
    if(type ==TOKEN_LPAREN)
        return '(';
    if(type ==TOKEN_RPAREN)
        return ')';

    //error, should have been given a operator but number or undefined
    exceptionTEST(3);
    return 0x00;
}


int precedanceCheck(char curOperator,char topStackOperator) {
    //returns 1 if current operators is higher precedance to current top
    //returns 0 if lower precedance to top
    //returns -1 if same precedance
    //returns 2 if curent operator is '(' or ')'
    //returns 3 if top is '('
    if(topStackOperator == '(')
        return 3;

    if(curOperator == '+'|| curOperator == '-') {
        if(topStackOperator == '+'|| topStackOperator == '-')
            return -1; // same precedance
        if(topStackOperator == '*'|| topStackOperator == '/')
            return 0; // lower precedance
    }

    if(curOperator == '*'|| curOperator == '/') {
        if(topStackOperator == '*'|| topStackOperator == '/')
            return -1; // same precedance
        if(topStackOperator == '+'|| topStackOperator == '-')
            return 1; // greater precedance
    }
    else {
        return 2;// current operator is a '(' or ')'
    }

    //thow error, either stack or curOperator is not valid
    exceptionTEST(4);

    //error
    return -1;

}

//Stack operations
void push(char operator, stack * st) {
    //Make sure top is not greater than size
    if(st->size <= st->top) {
        //error
        exceptionTEST(5);
    }
    st->top++;                        //increase top of stack
    st->stackArr[st->top] = operator; //save to stack                           TODO: possible NULL pointer

}
char pop( stack * st) {
    //check to see if stack is empty
    if(st->top == -1) { // stack is empty can't pop
        //error
        exceptionTEST(6);
    }
    char value = st->stackArr[st->top];
    st->top--;//delete top of stack
    return value;

}
char  peek(stack * st) {
    if(st->top ==-1) {
        //error
        exceptionTEST(6);
    }
    return st->stackArr[st->top]; // show top of stack
}

//error handling
void exceptionTEST(int type) {
    switch(type) {
        case 1:
            printf("UNDEFINED STRING TOKEN: TOKENIZE FUNC\n");
            exit(1);
        case 2:
            printf("GIVEN INPUT IS NOT A NUMBER: GETNUM FUNC\n");
            exit(2);
        case 3:
            printf("GIVEN INPUT IS NOT AN OPERATOR: GETOPERATORTYPE FUNC\n");
            exit(3);
        case 4:
            printf("GIVEN INPUT IS NOT A CORRECT TYPE OF OPERATOR: PRECEDANCECHECK FUNC\n");
            exit(4);
        case 5:
            printf("TRIED TO ADD TO STACK WHEN THERE WASN'T ENOUGH MEM, TOP>SIZE: PUSH FUNC \n");
            exit(5);
        case 6:
            printf("TRIED TO DELETE/ LOOK AT AN EMPTY STACK: POP OR PEEK FUNC \n");
            exit(6);
        default:
            printf("Undefined error\n");
            exit(-1);
    }
}
void panic(char* msg) {
    fprintf(stderr, "Fatal: %s\n", msg);
    abort();  // Generates a core dump
}

// Memory freeing function
void freeLL(token * ll) {
    //base case
    if(ll->next == NULL) {
        free(ll);
        return;
    }
    freeLL(ll->next);
}





