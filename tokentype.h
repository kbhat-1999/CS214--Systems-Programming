// Khushi Bhat .. Systems Programming Fall 2020
//
//
// defines for each of the Token types . For readability
#define WORD 48
#define DEC 1
#define OCT 2
#define HEX 3
#define FLOAT 4
#define LEFT_PARENTHESIS 5
#define RIGHT_PARENTHESIS 6
#define LEFT_BRACKET 7
#define RIGHT_BRACKET 8
#define STRUCT_MEMBER 9
#define STRUCT_PTR 10
#define SIZE_OF 11
#define COMMA 12
#define NEGATE 13
#define ONES_COMP 14
#define RIGHT_SHIFT 15
#define LEFT_SHIFT 16
#define BIT_XOR 17
#define BIT_OR 18
#define OR 18
#define INCREMENT 19
#define DECREMENT 20
#define ADD 21
#define DIV 22
#define LOGICAL_OR 23
#define LOGICAL_AND 24
#define CONDITIONAL_TRUE 25
#define CONDITIONAL_FALSE 26
#define EQ_TEST 27
#define INEQ_TEST 28
#define LESS_THAN 29
#define GREATER_THAN 30
#define LESS_OR_EQUAL 31
#define GREATER_OR_EQUAL 32
#define MOD_EQUALS 33
#define ASSIGN 34
#define PLUS_EQ 35
#define MINUS_EQ 36
#define TIMES_EQ 37
#define DIV_EQ 38
#define RIGHT_SHIFT_EQ 39
#define LEFT_SHIFT_EQ 40
#define BIT_AND_EQ 41
#define BIT_XOR_EQ 42
#define BIT_OR_EQ 43
#define AND 44
#define SUBTRACT 45
#define MULTIPLY 46
#define MOD  47

// Token Description array . Defines literal to be used for printing the token type
//
char * tokDescription []= {"decimal integer", "octal integer", "hexadecimal integer", "floating point", "left parenthesis", "right parenthesis", "left bracket", "right bracket", "structuremember", "structure pointer", "size of", "comma", "negate", "1s complement", "shift right", "shift left", "bitwise XOR", "bitwise OR", "increment", "decrement", "addition", "division", "logical OR", "logical AND", "conditional true", "conditional false", "equality test", "inequality test", "less than test", "greater than test", "less than or equal test", "greater than or equal test", "mod_equals", "assignment", "plus equals", "minus equals", "times equals", "divide equals", "shift right equals", "shift left equals", "bitwise AND equals", "bitwise XOR equals", "bitwise OR equals","AND/address operator", "minus/subtract operator", "multiply/dereference operator", "mod", "word"};