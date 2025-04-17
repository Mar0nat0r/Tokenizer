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
    char token;
    int size;
    int top;
    char *stack;
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
void postFix(int size, char *arr);
void push(char * arr, char * stack, int * top);
void pop(char * arr, char * stack, int * top);
void peek(char * arr, char * stack, int * top);

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



                                                        //free malloced memory
    freeLL(tokenLL);

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
void postFix(int size, char *arr) {}

//error handling
void exceptionTEST(int type) {
    switch(type) {
        case 1:
            printf("UNDEFINED STRING TOKEN: TOKENIZE FUNC\n");
            exit(1);
            break;
        case 2:
            printf("GIVEN INPUT IS NOT A NUMBER: GETNUM FUNC\n");
            exit(2);
        default:
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


