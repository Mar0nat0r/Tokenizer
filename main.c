#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
typedef enum {
    //Numbers
    TOKEN_NUMBER,

    //Basic operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_EXPONENT, //TODO impliment later

    //Parentheses
    TOKEN_LPAREN,
    TOKEN_RPAREN,

    //Functions
    TOKEN_NEGATE,
    TOKEN_SINE,
    TOKEN_COSINE,
    TOKEN_TANGENT,
    TOKEN_NATURAL_LOG,
    TOKEN_LOG,
    TOKEN_EXPONENTIAL,

    //Condition Checking
    TOKEN_EOF,
    TOKEN_UNDEFINED
}tokenType;
typedef struct token {
    tokenType type;
    int value; // use only if type == TOKEN_NUMBER
    struct token* next;
}token ;

                                                        // exception
int exceptionNum = 0;

void exceptionFunction(int type);
void panic( char* msg);


                                                        //Tokenizer
token *tokenize(char * input);
tokenType getToken(char character);


                                                        // Mem free
void freeLL(token * ll);

                                                        //supporting functions
int isNumber(char character);
int isOperator(char character);
int isFunction(char character);
int getNum(char * input, int * inputIDX, int Negative);


                                                        //stacks
char **postFix(token * ll);
char getOperatorType(tokenType type);
int precedanceCheck(char curOperator,char topStackOperator);

                                                        // Printing
int postfixEval(char ** arr);
void printPostfix(char ** arr);
void printPostfixNumber(char * arr, int i);
void printTokens(token * head);

void push(char operator, stack * st);
char  pop(stack * st);
char  peek(stack * st);

int main(void)
{
                                                         //get input and solution
    printf("Equation:");
    //TODO: fix -(), probably impliment a negative type
    char *input = " (66-78)+8574  *(54/3) - 78  \0"; //""2 + 3 * (4 - 1)\0";                    // string to interpret
    int i = 0;
    while(input[i] != '\0') {
        printf("%c",input[i]);
        i++;
    }
    printf("\n");
                                                        //get rid of spaces in input
                                                        //tokenized array & other set flag mem
    token *tokenLL = tokenize(input);
    char ** postfix = postFix(tokenLL);

    printTokens(tokenLL);
    printPostfix(postfix);

    int answer = postfixEval(postfix);

    printf("Answer to the Infix Equation: %d\n", answer);
                                                        //free malloced memory


    int IDX = 0;
    while(postfix[IDX] != NULL) {
        free(postfix[IDX]);
        IDX++;
    }
    free(postfix);
    printf("postfix freed...\n");
    freeLL(tokenLL);
    printf("token LL freed...\n");
    printf("done...\n");
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



        //blank space check
        if(input[i] == ' ') {
            i++;
            continue; // skip blank spaces, don't evaluate as a token
        }

        //number check
        if(isNumber(input[i])) {

            cur->type = TOKEN_NUMBER;
                                                                         // save number, account for numbers bigger than 1 digit
            cur->value = getNum(input,&i, 1);

            cur-> next = malloc(sizeof(token));                      //reserve space for next node
            cur-> next->value = 0;
            cur-> next -> next = NULL;
            cur = cur->next;
        }
        //operator check
        else if (isOperator(input[i])) {
            cur->type = getToken(input[i]);
                                                                            //error handling
            if(cur->type == TOKEN_UNDEFINED) {
                exceptionFunction(1);
                return head;
            }
                                                                            //check to make sure '-' sign is not a negative sign for a number
                                                                            // if it is, call getNum() and continue to next token
            // Indexes:                             [-1]  [0]   [1]
            // Subtraction Structure needs to be <Number> '-' <Number>
            if(cur->type == TOKEN_MINUS && isNumber(input[i-1]) && (isOperator(input[i+1]) || input[i+1] == '(') ) {
                cur-> type = TOKEN_NUMBER;
                i++;
                cur-> value = getNum(input, &i, -1);
            }
                                                                            //reserve space for next node
            cur->next = malloc(sizeof(token));
            cur-> next->value = 0;
            cur-> next -> next = NULL;
            cur = cur->next;

            i++;
        }

        //function check
        else {


        }


    }
    //end reached, make current node an end token
    cur->type = TOKEN_EOF;
    return head;
}
tokenType getToken(char  character) {
    switch (character) {
        case '\0':// NULL
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
int isOperator(char character) {
    if(character == '+' || character == '-' || character == '*' || character == '/' || character == '(' || character == ')' )
        return 1;
    return 0;
}
// TODO IMPLIMENT FUNCTIONS
int isFunction(char character) {
    //^ is exponent, e is exponential function, l is natural ln, L is log
    if(character == '^' || character == 'e' || character == 'l' || character == 'L')
        return 1;
    return 0;
}
int getNum(char * input, int * inputIDX, int Negative) {
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
        int j = 0;                                  //counter for tempNumArr IDX -> gives cur digit
        while(Digits != 0) {                       //create completed number using saved digits
            tempNum+= tempNumArr[j] * pow(10,Digits-1);                       //gets correct 10 exponent while getting correct spot in arr(digits-j)
            Digits--;                              // |--->go to next Less significant digit's place
            j++;                                   // |<--(multiply these to get number to add to final answer)
                                                   // |--->go to next numerical digit
        }
        tempNum *= Negative;                                  // Negative set to -1 if number is negative and 1 if positve
    }
    else {
                                                             // error handling, if given input is not a number
        exceptionFunction(2);
    }
    return tempNum;
}

//Postfix Function
char * *postFix(token *ll) {
    stack * opStack = malloc(sizeof(stack));
    token *temp = ll;
    char ** postfixArr;                                         //array of pointer to save each token
    char buffer[20];                                            // used to hold current number or buffer we are working with
    int size = -1;                                              // stack size tracker
    int postfixIDX = 0;                                         // for postfix arr
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
                                                                //properly size result, and stack arrays
    postfixArr = malloc(sizeof(char * )*size);                  // sized for 'char *' not 'char', remember!!
    opStack->stackArr = malloc(sizeof(char )*size);

    while(ll!=NULL) {                                           //while there are tokens to be read
                                                                //IF NUMBER
        if(ll->type ==TOKEN_NUMBER) {
            //convert the integer 'value' to string
            sprintf(buffer, "%d", ll->value);
            //size new index accordingly + NULL terminator
            //Note, strlen of buffer, not sizeof buffer
            postfixArr[postfixIDX] = malloc(strlen(buffer)+1);
            strcpy(postfixArr[postfixIDX], buffer);
            postfixIDX++;
        }
                                                                //IF OPERATOR
        else{
            //makes sure we don't overflow the stack
            if(opStack->top!=opStack->size) {

                curOperator = getOperatorType(ll->type);                     //grab current operator

                if(opStack->top == -1) {                                      // if stack is empty, PUSH current operator
                    push(curOperator, opStack);
                    //postfixIDX++;
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
                                    postfixArr[postfixIDX] = malloc(2); //operator & NULL terminator
                                    postfixArr[postfixIDX][0] = pop(opStack);
                                    postfixArr[postfixIDX][1] = '\0';
                                    postfixIDX++;                                    // go to next index in postfix arr
                                }                                                    // TODO: error if this doesn't break from the loop
                                opStack->top--;                                      // remove ')' from stack
                            }
                            else {
                                //curOperator is a starting bracket
                                push(curOperator,opStack);
                            }
                            break;
                        case 4: // reached end of string, continue
                            break;
                        case 3:                                                     //TODO top of stack is a '(' just continue
                            push(curOperator, opStack); // top is a '(' so we should just push the next thing onto it
                            break;
                        case 1: //current operators is higher precedance to current top
                            push(curOperator, opStack);
                            break;

                        case 0://cur is lower precedance to top
                            //pop until end of stack, '(' is seen, or if precedence is the same as the new top
                            while((opStack->top != -1) && (opStack->stackArr[opStack->top] != '(') && (precedanceCheck(curOperator, opStack->stackArr[opStack->top] ) != -1)) {
                                postfixArr[postfixIDX] = malloc(2); //operator & NULL terminator
                                postfixArr[postfixIDX][0] = pop(opStack);      //POP old operator
                                postfixArr[postfixIDX][1] = '\0';
                                postfixIDX++;
                            }
                            push(curOperator, opStack); //push new operator
                            break;

                        case -1: //Same precedance                      //TODO will impliment ^ after all infix->postfix code is working
                            postfixArr[postfixIDX] = malloc(2); //operator & NULL terminator
                            postfixArr[postfixIDX][0] = pop(opStack);      //POP old operator
                            postfixArr[postfixIDX][1] = '\0';
                            postfixIDX++;
                            push(curOperator,opStack);                  //Push new operator
                            break;
                        default:// should never reach here
                            exceptionFunction(4);
                    } //end of switch case
                } //end of if-else empty stack push check
            } // overflow stack check
        } //if-else number/operator check
        ll = ll->next;
    }// end of given LL


    //pop everything left on the stack
    while (opStack->top != -1) {
        postfixArr[postfixIDX] = malloc(2); //operator & NULL terminator
        postfixArr[postfixIDX][0] = pop(opStack);      //POP old operator
        postfixArr[postfixIDX][1] = '\0';
        postfixIDX++;
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
    if(type == TOKEN_EOF)
        return '\0';

    //error, should have been given a operator but number or undefined
    exceptionFunction(3);
    return 0x00;
}
int precedanceCheck(char curOperator,char topStackOperator) {
    //returns 1 if current operators is higher precedance to current top
    //returns 0 if lower precedance to top
    //returns -1 if same precedance
    //returns 2 if current operator is '(' or ')'
    //returns 3 if top is '(', just push current onto stack



    if( curOperator== '\0')
       return 4;

    if(curOperator == '+'|| curOperator == '-') {
        if(topStackOperator == '+'|| topStackOperator == '-')
            return -1; // same precedance
        if(topStackOperator == '*'|| topStackOperator == '/')
            return 0; // lower precedance
        if(topStackOperator == '(')
            return 3; // ignore precedance, push to stack
    }

    if(curOperator == '*'|| curOperator == '/') {
        if(topStackOperator == '*'|| topStackOperator == '/')
            return -1; // same precedance
        if(topStackOperator == '+'|| topStackOperator == '-')
            return 1; // greater precedance
        if(topStackOperator == '(')
            return 3; // ignore precedance, push to stack
    }
    else {
        return 2;// current operator is a '(' or ')'
    }

    //thow error, either stack or curOperator is not valid
    exceptionFunction(4);
    return 4;
}

//Stack operations
void push(char operator, stack * st) {
    //Make sure top is not greater than size
    if(st->size <= st->top) {
        //error
        exceptionFunction(5);
    }
    st->top++;                        //increase top of stack
    st->stackArr[st->top] = operator; //save to stack                           TODO: possible NULL pointer

}
char pop( stack * st) {
    //check to see if stack is empty
    if(st->top == -1) { // stack is empty can't pop
        //error
        exceptionFunction(6);
    }
    char value = st->stackArr[st->top];
    st->top--;//delete top of stack
    return value;

}
char  peek(stack * st) {
    if(st->top ==-1) {
        //error
        exceptionFunction(6);
    }
    return st->stackArr[st->top]; // show top of stack
}

//error handling
void exceptionFunction(int type) {
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
        case 7:
            printf("MEM ALLOCATION FAILED; POSTFIXEVAL FUNC \n");
            exit(7);
        case 8:
            printf("Wrong operator in stack somehow; POSTFIXEVAL FUNC \n");
            exit(8);
        case 9:
            printf("ANSWER ARRAY INDEX WENT NEGATIVE SHOULD NOT HAPPEN; POSTFIXEVAL FUNC \n");
            exit(9);
        case 10:
            printf(" <UNDEFINED> DIVIDED BY NEGATIVE NUMBER: POSTFIXEVAL FUNC\n");
        exit(10);
        case 11:
            printf(" UNDEFINED TOKEN: PRINTTOKEN FUNC\n");
        exit(11);
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

void printPostfix(char ** arr) {
    int i = 0;
    printf("Postfix arr:");
    while(arr[i] != NULL) {
        if(isNumber(arr[i][0]) ) {
            printPostfixNumber(arr[i], 0);
            printf(" ");
            i++;
        }
       else {// is operator
           printf("%s ",arr[i]);
           i++;
       }
    }
    printf("\n");
}
void printPostfixNumber(char * arr, int i) {
    // base case, if string[i] == \0, end
    if(arr[i] == '\0')
        return;
    printf("%c",arr[i]);
    printPostfixNumber(arr, i+1);
}

void printTokens(token * head) {
    token * temp = head;
    printf("Tokens: ");
    while(temp->type != TOKEN_EOF) {
        //error
        if(temp->type == TOKEN_UNDEFINED)
            exceptionFunction(11);
        //token is an operator
        if(temp->type != TOKEN_NUMBER) {
            tokenType curType = temp -> type;
            if(curType == TOKEN_DIV )
                printf("'/', ");
            if(curType == TOKEN_MUL )
                printf("'*', ");
            if(curType == TOKEN_MINUS )
                printf("'-', ");
            if(curType == TOKEN_PLUS )
                printf("'+', ");
            if(curType == TOKEN_LPAREN )
                printf("'(', ");
            if(curType == TOKEN_RPAREN )
                printf("')', ");

        }
        //token is a number
        else
            printf("'%d', ", temp-> value);

        temp = temp->next;

    }

    printf("'EndToken'\n");
}

//Postfix evaluation
int postfixEval(char ** arr) {
    int IDX = 0; // pointer index within arr
    int j; // character index within arr
    int * getNumIDX = &j;
    int temp = 0; // used for calculations

    int i = 0;// used for size of answer arr
    //get size of array

    while(arr[IDX] != NULL) {
        i++;
        IDX++;
    }

    int *answerArr = malloc(sizeof(int) * i);
    IDX = 0;
    i = 0;
    if (!answerArr) {
        // handle memory allocation failure
        exceptionFunction(7);
    }


    while(arr[IDX] != NULL ) {
        //if (answerArr != NULL)// answerArr should never be NULL
        //    exceptionTEST(8);

        *getNumIDX = 0; //rest index inside each indiviual pointer for the int arr
        // go through each arr idx
        // -> check if it's a number, make sure to include negative numbers

        // is a number if true
        if(isNumber(arr[IDX][0])  ){
            //   * save whole number to index
            answerArr[i] = getNum(arr[IDX], getNumIDX , 1);
            IDX++;
            i++;
        }

        //Negative check
        //0. mak
        //1. It's the first item in the arr
        //2. It comes after another operator
        //3. Is followed by a number
        //Negative Structure <Operator> '-' <Number>
        //Subtration Structure <Number> '-' <Number>
        else if(arr[IDX][0] == '-') {  //0. char is '-'
            if(IDX ==0) { //1. '-' is first char in array meaning it is part of a negative number
                answerArr[i] = getNum(arr[IDX], getNumIDX, -1);
                IDX++;
                i++;
            }
            //       2. preceding token is an operator       3. '-' is followed by a number
            else if( isOperator(arr[IDX-1][0])        &&     isNumber(arr[IDX][1]) ) {
                answerArr[i] = getNum(arr[IDX], getNumIDX, -1);
                IDX++;
                i++;
            }

            else {// '-' is not a negative, but a subtraction sign
                temp =  answerArr[i-2] - answerArr[i-1];
                IDX++;
                i--;    //decrease the index of the answer stack, two numbers have been operated on to become 1
                answerArr[i-1] = temp;
            }

        }


        // is an operator
        else { //   * if op, pop 2 numbers, then save the result to the stack
            if(arr[IDX][0] == '+')
                temp = answerArr[i-2] + answerArr[i-1];
            else if(arr[IDX][0] == '*')
                temp = answerArr[i-2] * answerArr[i-1];
            else if(arr[IDX][0] == '/') {
                if(answerArr[i-1] == 0)
                    exceptionFunction(10);
                temp =  answerArr[i-2] / answerArr[i-1];
            }
            // No need for '-' check since it was checked in previous else if statement
            else {
                //error should not occur
                exceptionFunction(8);
            }

            IDX++;  // go to next item in postfix stack
            i--;    //decrease the index of the answer stack, two numbers have been operated on to become 1
            answerArr[i-1] = temp;

        }
        if(i < 0) {
            //error, i should never be less than 0 since it is used as the index variable for the answer array
            exceptionFunction(9);
        }


    }
    int answer = answerArr[0];

    //TODO: fix mem alocation
    free(answerArr);

    return answer;
}
