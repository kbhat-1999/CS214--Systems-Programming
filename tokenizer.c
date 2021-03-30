// Khushi Bhat .. Systems Programming Fall 2020
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "tokentype.h"  // includes the locally found file tokentype.h

/* printtoken
 * Prints the token after its final type has been determined
 * Takes in the token type and the token string as input
 * Utilizes the array definition in tokDescription (defined inthe include file) to print
 */

int printToken(int tokenType, char* tokenStr) {
        //tokenType is the final token type of the token. tokenStr is the token that will be printed along with the token type.
        printf("%s: \"%s\"\n", tokDescription[tokenType-1], tokenStr);
}


/* findStartTokenType
 * Given a input character , this will determine the starting token type ,
 * Returns the tokenType
 * IMP NOTE : should be called and is called only at start of new token
 * NOTE: In main logic This token is revaluate as needed and may be updated when necessary later
 */

int findStartTokenType (int c) {
        int tokenType;
        if (isalpha(c)) {   // alpha is WORD
                tokenType = WORD;
        }
        else if(isdigit(c)) { // number is OCT is starting is 0 , else a DEC
                if (c == '0')
                        tokenType = OCT;
                else
                        tokenType = DEC;
        }
        else if (c == '(') {
                tokenType = LEFT_PARENTHESIS;
        }
        else if (c == ')') {
                tokenType = RIGHT_PARENTHESIS;
        }
        else if (c == '[') {
                tokenType = LEFT_BRACKET;
        }
        else if (c == ']') {
                tokenType = RIGHT_BRACKET;
        }
        else if (c == '.') {
                tokenType = STRUCT_MEMBER;
        }
        else if (c == ',') {
                tokenType = COMMA;
        }
        else if (c == '!') {
                tokenType = NEGATE;
        }
        else if (c == '~') {
                tokenType = ONES_COMP;
        }
        else if (c == '^') {
                tokenType = BIT_XOR;
        }
        else if (c == '|') {
                tokenType = BIT_OR;
        }
        else if (c == '+') {
                tokenType = ADD;
        }
        else if (c ==  '/') {
                tokenType = DIV;       
        }       
        else if (c == '?') {
                tokenType = CONDITIONAL_TRUE;
        }
        else if (c == ':') {
                tokenType = CONDITIONAL_FALSE;
        }
        else if (c == '<') {
                tokenType = LESS_THAN;
        }
        else if (c == '>') {
                tokenType = GREATER_THAN;
        }
        else if (c == '=') {
                tokenType = ASSIGN;
        }
        else if (c == '&') {
                tokenType = AND;
        }
        else if (c == '-') {
                tokenType = SUBTRACT;
        }
        else if (c == '*') {
                tokenType = MULTIPLY;
        }
        else if (c == '%') {
                tokenType = MOD;
        }
        /* printf("%c %d %c", c, tokenType, tokenType);
        printf("%c ", c);
        printf("\"%s\"\n", tokDescription[tokenType-1]);
        */
        return tokenType;
}

/* isStartofToke
 * checks if this character is start of Token . Atpresent a non whitespace is a start of token
 * returns false if this char does not start a new token
 * returns true if this char starts of a new token
 */
bool isStartofToken(char c)
{
        if (isspace(c))
                return false;
        return true;
}
/* evaluateTokenAndEndToken
 * checks if token has ended or if a token type changes from current type
 * takes in the character and the current tokentype as input
 * token type is passed by address and hence is changed here as needed.
 * returns true if token has ended ,
 * returns false is token has not ended. as mentioned , tokenType may also change
 */
bool evaluateTokenAndEndToken(char c, int *tokenType)
{

        //printf("\nevaluate %c \n", c);
        // if char is a space or end of string , the token has obviously ended
        if (isspace(c) || c == '\0' )   
                return true;

        // if the character is alpha
        if (isalpha(c)) {
                    //printf("alpha %c\n",c);
            if (*tokenType == WORD)  // iif token is already WORD
                    return false; // word continues
            else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) { // handles cases where token is a HEX so far
                    if (*tokenType == HEX)
                        return false;  // hex continues
                   else
                    return true; // new token begins
            }

            else if (c == 'e') { // handles cases where token is a FLOAT  so far
                   if (*tokenType == FLOAT)
                    return false;  // float continues
                   else
                    return true; // new token begins
            }
            else if (c == 'x' || c == 'X') { // Converts .. may not be needed  since isdigit block will handle this
                if (*tokenType == OCT) {
                    *tokenType = HEX;
                        return false;
                }
                else
                        return true;
            }
            else if (*tokenType != WORD) { // if noone of the above is true, then a new  token would start
                    return true;
            }
        }
        //
        //
        // if the character is digit
        if (isdigit(c)) {
                if (*tokenType == DEC  || *tokenType == HEX || *tokenType == FLOAT)
                        return false;  // Decimal or Float or hex continues
                else if (*tokenType == OCT ) {
                        if (c > '7') {
                                *tokenType = DEC; // no longer Octal but a Decimal
                        }
                        return false;
                }
                else if (*tokenType == WORD) // if the digit is part of a WORD
                        return false;
                else
                        return true;  // if none of the above is true , then a new token would start
        }


        // For all other cases (non whitespace, non null, non alpha and non digit
        switch (c)
        {
         case '(':
         case ')':
         case '[':
         case ']':
         case ',':
         case '~':
         case '?':
         case ':':
         case '/':
         case '*':
         case '%':
         case '^':
         case '!':
                return true;  //for all othe above , previous token ends and a new on will being , returns true
         case '+':
                if (*tokenType == ADD) {  // if token is an ADD , then a new '+' would make it an INCREMENT
                        *tokenType = INCREMENT;
                        return false;
                }
                return true;
         case '-':
                if (*tokenType == SUBTRACT) {// if token is an SUBTRACT , then a new '-' would make it an DECREMENT
                        *tokenType = DECREMENT;
                        return false;
                }
                else if (*tokenType == FLOAT) {
                        *tokenType = FLOAT;
                        return false;
                }
                return true;
         case '=':  // handle all the cases where = is a follow up character anc tokenType needs to be converted
                if (*tokenType == ASSIGN) {  // for ==
                        *tokenType = EQ_TEST;
                        return false;
                }
                else if (*tokenType == NEGATE) {// for !=
                        *tokenType = INEQ_TEST;
                        return false;
                }
                else if (*tokenType == LESS_THAN) {// for <=
                        *tokenType = LESS_OR_EQUAL;
                        return false;
                }
                else if (*tokenType == GREATER_THAN ) {// for >=
                        *tokenType = GREATER_OR_EQUAL;
                        return false;
                }
                else if (*tokenType == ADD) {// for +=
                        *tokenType = PLUS_EQ;
                        return false;
                }
                else if (*tokenType == SUBTRACT) {// for -=
                        *tokenType = MINUS_EQ;
                        return false;
                }
                else if (*tokenType == MULTIPLY) {// for *=
                        *tokenType = TIMES_EQ;
                        return false;
                }
                else if (*tokenType == MOD) {// for %=
                        *tokenType = MOD_EQUALS;
                        return false;
                }
                else if (*tokenType == RIGHT_SHIFT) {// for <<=
                        *tokenType = RIGHT_SHIFT_EQ;
                        return false;
                }
                else if (*tokenType == LEFT_SHIFT) {// for >>=
                        *tokenType = LEFT_SHIFT_EQ;
                        return false;
                }
                else if (*tokenType == AND) {// for &=
                        *tokenType = BIT_AND_EQ;
                        return false;
                }
                else if (*tokenType == BIT_XOR) {// for ^=
                        *tokenType = BIT_XOR_EQ;
                        return false;
                }
                else if (*tokenType == BIT_OR) {// for |=
                        *tokenType = BIT_OR_EQ;
                        return false;
                }
                return true;
         case '&':
                if (*tokenType == AND) {// for &&
                        *tokenType = LOGICAL_AND;
                        return false;
                }
                return true;
         case '|':
                if (*tokenType == OR) { // for ||
                        *tokenType = LOGICAL_OR;
                        return false;
                }
                return true;
         case '.':
                if (*tokenType == DEC || *tokenType == OCT) {  // Convert DEC and OCT to FLOAT if a decimal point is found
                        *tokenType = FLOAT;
                        return false;
                }
                return true;
         case '>':
                if (*tokenType == SUBTRACT) { // for ->
                        *tokenType = STRUCT_PTR;
                        return false;
                }
                else if (*tokenType == GREATER_THAN) {  // for >>
                        *tokenType = RIGHT_SHIFT;
                        return false;
                }
                return true;
         case '<':
                if (*tokenType == LESS_THAN) { // for <<
                        *tokenType = LEFT_SHIFT;
                        return false;
                }
                return true;
         default:

                return false;
        }
        return false;
}

/* main
 * take in arguments from the command line as input
 * Processes the string and creates string
 * preserves original string
 * Each token s created into a char array called tokenStr. 
 */

int main (int argc, char** argv) {
        if (argc != 2) { //argv[0] = ./tokenizer, argv[1] is the input string that will be tokenized
                printf("incorrect usage\n");
                printf("Usage : %s \"string to tokenize\"\n", argv[0]); 
                return 1;
        }
        int tokenType; // tracks current tokenType.
        char *inputStr ; //Input String
        char *tokenStr ; // token string
        int inputStrIndex = 0; // index into the inputStr array
        int tokenStrIndex = 0; // index into the tokenStr array
        bool tokenStarted ; // kkeps track of whether a new token has started

        inputStr = (char *) malloc(strlen(argv[1])+1);  // allocate memory for input string
        strcpy(inputStr, argv[1]);  // Copy argv[1] to the inputStr

        tokenStr = (char *) malloc(strlen(inputStr)+1);  // allocate memory for token . size for worst case there input string is one token

        tokenType = 0; // initialize to nothing
        tokenStr[0] = '\0';
        tokenStarted = false; // initializr current status to token Not started

        //printf ("\n\n-------\n%s  \"%s\"\n", argv[0],inputStr);
        // step thru each char of input string from left to right and process it
        for (inputStrIndex = 0; inputStr[inputStrIndex] != '\0'; inputStrIndex++) {
                if (! tokenStarted) {  // if token has not started yet
                       if (isStartofToken(inputStr[inputStrIndex])) {  // check if this character starts a new token  and if so do this block
                          tokenStarted = true;
                          tokenStrIndex = 0;
                          tokenStr[tokenStrIndex++]=inputStr[inputStrIndex]; // assign the character to the start of tokenStr
                          tokenType = findStartTokenType(inputStr[inputStrIndex]); // find start Token type
                       }
                       else {  //  not yet a new token , likely just whitespace
                          continue;
                       }
                }
                else {  // if a token has already been started
                       if (evaluateTokenAndEndToken(inputStr[inputStrIndex], &tokenType)) {  // check if the token ends or toke type needs to change
                         tokenStr[tokenStrIndex] = '\0';   // end token string
                         if (strcmp(tokenStr,"sizeof") == 0 )
                              tokenType = SIZE_OF;
                         printToken(tokenType, tokenStr);  // print the token
                         tokenStarted = false;   // set token Started to false
                         if (isStartofToken(inputStr[inputStrIndex])) { // if a new token can now start .. takes care of whitespace cases
                             tokenStarted = true;
                             tokenStrIndex = 0;
                             tokenStr[tokenStrIndex++]=inputStr[inputStrIndex];
                             tokenType = findStartTokenType(inputStr[inputStrIndex]);
                         }
                       }
                       else { // token has not ended yet , so add to token str , increment index and continue processing
                          tokenStr[tokenStrIndex++]=inputStr[inputStrIndex];
                          continue;
                       }
                }

        }
        if (tokenStarted != false) {  // for the last token
           tokenStr[tokenStrIndex] = '\0';   // end token string
           if (strcmp(tokenStr,"sizeof") == 0 )// override token type from WORD to SIZE_OF for sizeof
               tokenType = SIZE_OF;
           printToken(tokenType, tokenStr);
        }
        return 0;

}
