/*
 * Student name : Khushi Bhat
 */

#include <pthread.h>
// structure used to pass arguments to the dirHandler function
// since a thread function can take only one argument, we  create a strcture
struct dirHandlerArgs {
    char *dirName;  // pointer to the directory name
    struct shareMemStruct *shm; // pointer to shared memory strucutre
    pthread_mutex_t *mymutex ; // mutex used for locking
};

// structure used to pass argument to the fileHandler fuction
struct fileHandlerArgs {
    char *fileName; // pointer to the filename
    struct shareMemStruct *shm; // pointer to the shared memory structure
    pthread_mutex_t *mymutex ; // mutex used for locking
};

// structure used for each token and the token count. This is will a horizontal linked list
// head of the linked list will the a filenode for the file and each subsquent link is
// a sorted token list
struct tokenNode {
    char *token; // token
    double  tokenCountinFile; // count  and ratio of a given token in the file
    struct tokenNode *nextToken; // pointer to the next token
};

// structure used for the files examined in a run. Each file node is added to the linked
// list by the fileHandler. This is a vertical linked list .
struct fileNode {
    char *fileName; // pointer to file name
    long totalTokensInFile; // total tokens in the file
    struct tokenNode *firstToken; // pointer to the first token in the file
    struct fileNode *nextFile; // pointer to the next File
};

//the shared memory structure
//just has one member, but can have more if needed
struct shareMemStruct {
    struct fileNode *firstFileNode;
};
struct JSD {
        struct fileNode *first;
        struct fileNode *second;
        long totalTokenCount;
        double score;
        struct JSD *next;
};
#define MAX_LINE_SIZE_IN_FILE 1024
#define MAX_TOKEN_SIZE 51
#define STRINGTOK_SEP "<>| `~@#$%^{}[]=+-()&;:/?\t,.!\n\"\'*"
char *topdirName;
void* dirHandler (void* args) ;
void* fileHandler (void* args) ;
int tokenizer(struct fileNode *fileNode) ;
double fileCompComputePrint(struct fileNode *first, struct fileNode *second);
void printJSD(struct JSD *allJSD);
