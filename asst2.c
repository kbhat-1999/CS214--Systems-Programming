/*
 * Student name : Khushi Bhat
 */
#include "asst2.h"

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* dumpfileNode
 * helper diagnostic function, dumps details of all tokens in afile node
 */
void dumpfileNode(struct fileNode *fileNode) {
        struct tokenNode *currTokenNode ;
        printf ("%s %ld \n------------\n", fileNode->fileName, fileNode->totalTokensInFile);
        currTokenNode = fileNode->firstToken;
        while (currTokenNode != NULL) {
                printf ("\t%s %f\n",currTokenNode->token, currTokenNode->tokenCountinFile);
                currTokenNode = currTokenNode->nextToken;
        }
}

/* dumpSHM
 * helper diagnostic function, dumps the shared memory structure
 */
void dumpSHM(struct shareMemStruct *shm) {
    struct fileNode *currFileNode;
    printf("\nShared Global Memory\n");
    currFileNode =     shm->firstFileNode;
    while (currFileNode != NULL) {
            dumpfileNode(currFileNode);
            currFileNode = currFileNode->nextFile;
    }
}

/* freefileNode
 * helper diagnostic function, frees all tokens in afile node
 */
void freefileNode(struct fileNode *fileNode) {
        struct tokenNode *currTokenNode ;
        struct tokenNode *prevTokenNode ;
        currTokenNode = fileNode->firstToken;
        while (currTokenNode != NULL) {
                prevTokenNode = currTokenNode;
                currTokenNode = currTokenNode->nextToken;
                free(prevTokenNode);
        }
}

/* freeSHM
 * helper diagnostic function, frees the shared memory structure
 */
void freeSHM(struct shareMemStruct *shm) {
    struct fileNode *currFileNode;
    struct fileNode *prevFileNode;
    currFileNode =     shm->firstFileNode;
    while (currFileNode != NULL) {
            freefileNode(currFileNode);
            currFileNode = currFileNode->nextFile;
    }
    currFileNode =     shm->firstFileNode;
    while (currFileNode != NULL) {
                prevFileNode = currFileNode;
                currFileNode = currFileNode->nextFile;
                free(prevFileNode);
    }
    free(shm);
}



/* fileCompComputePrint
 * given two files, compare using given algorithm and print results
 */

double fileCompComputeJSD(struct fileNode *first, struct fileNode *second ) {
        //printf ("%s %s\n", first->fileName, second->fileName);
        double firstFileKLD, secondFileKLD, jsd, f ,s;
        struct tokenNode *currTokenNodeFirstFile, *currTokenNodeSecondFile;
        int cmpCode = 0;
        currTokenNodeFirstFile = first->firstToken;
        currTokenNodeSecondFile = second->firstToken;
        firstFileKLD = 0;
        secondFileKLD = 0;

        while (1) {
                //if first and second are both not null
                if (currTokenNodeFirstFile != NULL && currTokenNodeSecondFile != NULL) {
                        //compare first and second
                        cmpCode=strcmp(currTokenNodeFirstFile->token, currTokenNodeSecondFile->token);
                        if (cmpCode == 0) {
                        //if same,
                                //compute KLD for first
                                f=currTokenNodeFirstFile->tokenCountinFile*log10(currTokenNodeFirstFile->tokenCountinFile/((currTokenNodeFirstFile->tokenCountinFile+currTokenNodeSecondFile->tokenCountinFile)/2));
                                firstFileKLD+=f;
                                //compute KLD for second
                                s=currTokenNodeSecondFile->tokenCountinFile*log10(currTokenNodeSecondFile->tokenCountinFile/((currTokenNodeFirstFile->tokenCountinFile+currTokenNodeSecondFile->tokenCountinFile)/2));
                                secondFileKLD+=s;
                        //printf("%s %s %f %f %f %f\n",currTokenNodeFirstFile->token,currTokenNodeSecondFile->token, f, s, firstFileKLD, secondFileKLD);
                                //advance both
                                currTokenNodeFirstFile=currTokenNodeFirstFile->nextToken;
                                currTokenNodeSecondFile=currTokenNodeSecondFile->nextToken;
                        }
                        else if (cmpCode > 0) {
                        //if second is behind
                                //compute KLD for second
                                s=currTokenNodeSecondFile->tokenCountinFile*log10(currTokenNodeSecondFile->tokenCountinFile/((0+currTokenNodeSecondFile->tokenCountinFile)/2));
                                secondFileKLD+=s;
                        //printf("%s %s * %f %f %f\n",currTokenNodeFirstFile->token,currTokenNodeSecondFile->token, s, firstFileKLD, secondFileKLD);
                                //advance second
                                currTokenNodeSecondFile=currTokenNodeSecondFile->nextToken;
                        }
                        else {
                        //if first is behind
                                //compute KLD for first
                                f=currTokenNodeFirstFile->tokenCountinFile*log10(currTokenNodeFirstFile->tokenCountinFile/((currTokenNodeFirstFile->tokenCountinFile+0)/2));
                                firstFileKLD+=f;
                        //printf("%s %s %f * %f %f\n",currTokenNodeFirstFile->token,currTokenNodeSecondFile->token, f, firstFileKLD, secondFileKLD);
                                //advance first
                                currTokenNodeFirstFile=currTokenNodeFirstFile->nextToken;
                        }
                }
                else if (currTokenNodeFirstFile == NULL &&  currTokenNodeSecondFile == NULL) {
                        break;
                }
                else if (currTokenNodeFirstFile == NULL) {
                //if first is null
                        //just do second
                                //compute KLD for second
                                s=currTokenNodeSecondFile->tokenCountinFile*log10(currTokenNodeSecondFile->tokenCountinFile/((0+currTokenNodeSecondFile->tokenCountinFile)/2));
                                secondFileKLD+=s;
                        //printf("***** %s * %f %f %f\n",currTokenNodeSecondFile->token, s, firstFileKLD, secondFileKLD);
                                //advance second
                                currTokenNodeSecondFile=currTokenNodeSecondFile->nextToken;
                }
                else if (currTokenNodeSecondFile == NULL) {
                //if second is null
                        //just do first
                                //compute KLD for first
                                f=currTokenNodeFirstFile->tokenCountinFile*log10(currTokenNodeFirstFile->tokenCountinFile/((currTokenNodeFirstFile->tokenCountinFile+0)/2));
                                firstFileKLD+=f;
                        //printf("%s *******  %f * %f %f\n",currTokenNodeFirstFile->token,f, firstFileKLD, secondFileKLD);
                                //advance first
                                currTokenNodeFirstFile=currTokenNodeFirstFile->nextToken;
                }
        }

        jsd =  (firstFileKLD+secondFileKLD)/2;
        return jsd;
        return 0;
}

/*
 * printJSD
 * prints the JSD for a file pair in the order of lowest totalTokenCount for the two to the highest
 *
 */
void printJSD(struct JSD *allJSD) {
        struct JSD *currentJSD  = allJSD;
        while (currentJSD != NULL) {
                // print in appropriate color
                if  (currentJSD->score >= 0 && currentJSD->score < 0.1) {
                        printf("\033[31m"); // red
                        printf("%0.2f\t%s and %s\n",currentJSD->score, currentJSD->first->fileName, currentJSD->second->fileName);
                }
                else if  (currentJSD->score >= 0.1 && currentJSD->score < 0.15) {
                        printf("\033[33m"); // yellow
                        printf("%0.2f\t%s and %s\n",currentJSD->score, currentJSD->first->fileName, currentJSD->second->fileName);
                }
                else if  (currentJSD->score >= 0.15 && currentJSD->score < 0.2) {
                        printf("\033[32m"); // green
                        printf("%0.2f\t%s and %s\n",currentJSD->score, currentJSD->first->fileName, currentJSD->second->fileName);
                }
                else if  (currentJSD->score >= 0.2 && currentJSD->score < 0.25) {
                        printf("\033[36m"); // cyan
                        printf("%0.2f\t%s and %s\n",currentJSD->score, currentJSD->first->fileName, currentJSD->second->fileName);
                }
                else if  (currentJSD->score >= 0.25 && currentJSD->score < 0.3) {
                        printf("\033[34m"); // blue
                        printf("%0.2f\t%s and %s\n",currentJSD->score, currentJSD->first->fileName, currentJSD->second->fileName);
                }
                else  {
                        printf("\033[0m"); // reset the colors
                        printf("%0.2f\t%s and %s\n",currentJSD->score, currentJSD->first->fileName, currentJSD->second->fileName);
                }
                currentJSD = currentJSD->next;
        }
}

/* calcTokenRatio
 * caluculates Token ration for each token in a given file
 * called by fileHandler for each file
 */
void calcTokenRatio(struct fileNode *fileNode) {
        struct tokenNode *currTokenNode ;
        currTokenNode = fileNode->firstToken;
        while (currTokenNode != NULL) {
                if (fileNode->totalTokensInFile != 0)
                        currTokenNode->tokenCountinFile = currTokenNode->tokenCountinFile / fileNode->totalTokensInFile;
                else
                        currTokenNode->tokenCountinFile = 0;
                currTokenNode = currTokenNode->nextToken;
        }
}


/*
 * tokenizer
 * Takes in the a fileNode struct as an argument
 * return 0 on success
 *
 * Does the following
 * tokens the file and add to the structure
 * sorts the token as it insert
 * computes discrete probabily of each token at the end
 */
int tokenizer(struct fileNode *fileNode) {

        FILE *fp;
        char line[MAX_LINE_SIZE_IN_FILE];
        char *token;
        char *saveptr;
        struct tokenNode *currTokenNode, *prevTokenNode,*nt;


        //printf("Tokenizing %s\n", fileNode->fileName);
        //open file for read
        fp = fopen(fileNode->fileName , "r");
        if (fp == NULL) {
            printf("Error opening file %s",fileNode->fileName);
            return(-1);
        }
        while (fgets (line, MAX_LINE_SIZE_IN_FILE, fp)!=NULL ) {
                    // get token from current line
                    token = strtok_r(line,STRINGTOK_SEP,&saveptr);
                    while (token != NULL) {
                        for(int i = 0; token[i]; i++){
                             token[i] = tolower(token[i]);
                        }
                        //printf("[%s]\n",token);
                        // for a token , update the token list for this file
                        // start from the head/begining for every token
                        currTokenNode = fileNode->firstToken;
                        // if currTokenNode is Null , first token  for file , add it
                        if (currTokenNode == NULL) {
                                currTokenNode = (struct tokenNode  *) malloc (sizeof(struct tokenNode ));
                                fileNode->firstToken = currTokenNode;
                                currTokenNode->nextToken = NULL;
                                currTokenNode->token = (char *) malloc(sizeof(token));
                                strcpy(currTokenNode->token, token);
                                currTokenNode->tokenCountinFile = 1;
                                fileNode->totalTokensInFile=1;
                        }
                        else {
                                int i=0;
                                while(1) {
                                        // compare tokens using strcmp
                                        i=strcmp(token,currTokenNode->token);
                                        //printf("\t[%s] [%s] %d\n",token, currTokenNode->token,i);
                                        // if equal , add to count since same token
                                        if (i==0 ) {
                                                currTokenNode->tokenCountinFile++;
                                                fileNode->totalTokensInFile++;
                                                break;
                                        }
                                        // if <0 insert the new link
                                        else if (i<0) {
                                                nt = (struct tokenNode  *) malloc (sizeof(struct tokenNode ));
                                                if (currTokenNode == fileNode->firstToken) {
                                                        fileNode->firstToken = nt;
                                                        nt->nextToken = currTokenNode;
                                                }
                                                else {
                                                        nt->nextToken = prevTokenNode->nextToken;
                                                        prevTokenNode->nextToken = nt;
                                                }

                                                nt->token = (char *) malloc(sizeof(token));
                                                strcpy(nt->token, token);
                                                nt->tokenCountinFile = 1;
                                                fileNode->totalTokensInFile++;
                                                break;
                                        }
                                        // if >0 , check next token in list
                                        else {
                                                if (currTokenNode->nextToken == NULL) { // at end of list and match not found, add it
                                                        nt = (struct tokenNode  *) malloc (sizeof(struct tokenNode ));
                                                        nt->nextToken = NULL;
                                                        currTokenNode->nextToken = nt;
                                                        nt->token = (char *) malloc(sizeof(token));
                                                        strcpy(nt->token, token);
                                                        nt->tokenCountinFile = 1;
                                                        fileNode->totalTokensInFile++;
                                                        break;
                                                }
                                                else {
                                                        prevTokenNode = currTokenNode;
                                                        currTokenNode = currTokenNode->nextToken;
                                                }
                                        }

                                }
                        }
                        token = strtok_r(NULL,STRINGTOK_SEP,&saveptr);
                    }
        }
        fclose(fp);
        return 0;
}

/*
 * fileHandler - File handler
 * Takes structure with arguments as input
 * Takes a file as ainput
 * Adds it to the shared meme structure,
 * Tokensizes the file , which adds the tokens to the shared mem
 */
void* fileHandler (void* args) {

        struct fileHandlerArgs *arg = (struct fileHandlerArgs *) args;
        struct fileNode *fn=NULL;
        struct fileNode *currFileNode=NULL;


        //printf("Running fileHandler for [%s] \n", arg->fileName);
        // check if he file is accessible, return if not
        if (access(arg->fileName,R_OK) != 0) {
                printf("file %s is not accessible \n", arg->fileName);
                //pthread_exit((void*) 0);
                return ((void *) 1);
        }

        // create the necessary data strucutures and intialize them
        fn = (struct fileNode *) malloc (sizeof(struct fileNode ));
        fn->fileName = arg->fileName;
        fn->totalTokensInFile = 0;
        fn->firstToken = NULL;
        fn->nextFile = NULL;
        //sleep(20);
        // obtain the thread sync mutex lock
        pthread_mutex_lock (arg->mymutex);
        //
        // create a newentry fo this file in the shared memory structure
        // will be at the end of this linked list
        //
        if (arg->shm->firstFileNode == NULL)
                arg->shm->firstFileNode = fn;
        else {
                currFileNode = arg->shm->firstFileNode;
                while (currFileNode->nextFile != NULL)
                        currFileNode = currFileNode->nextFile;
                currFileNode->nextFile = fn;
        }
        // release the thread sync mutex lock since sone with syncronized work
        pthread_mutex_unlock (arg->mymutex);

        // call tokenizer with pointer to this node
        tokenizer(fn);
        // Calclulate token ration for rach token in the file
        calcTokenRatio(fn);
        //pthread_exit((void*) 0);
        return ((void *) 0);

}

/*
 * dirHandler - Directory handler
 * Takes structure with arguments as input
 * creates a thread of dirHandler if a dirent is a directory
 * creates afirstToken thread of filehandler if a dirent is a file
 */
void* dirHandler (void* args) {

        struct dirHandlerArgs *arg = (struct dirHandlerArgs *) args;
        pthread_t thread[1024];
        int threadCount=0;
        pthread_attr_t threadAttrs;
        DIR *dir;
        struct dirent *dirEnt;
        struct stat path_stat;
        struct dirHandlerArgs dirArgs[1024];
        struct fileHandlerArgs fileArgs[1024];
        char   *pathName;
        //printf("Running dirHandler for [%s]\n", arg->dirName);
        //check if directory is accessible
        if (access(arg->dirName,R_OK | X_OK) != 0) {
                printf("directory %s is not accessible \n", arg->dirName);
                return ((void *) 1);
        }


        //opendir with opendir()
        if ((dir=opendir(arg->dirName)) != NULL) {
                pthread_attr_init(&threadAttrs);
                //loopthru the directory entries
                while((dirEnt=readdir(dir)) != NULL) {
                        // ignore "." and ".."
                        if (strcmp(dirEnt->d_name,".") == 0 || strcmp(dirEnt->d_name,"..") == 0)
                                continue;
                        //printf("%s\n",dirEnt->d_name);
                        //construct path
                        pathName = (char *) malloc(strlen(arg->dirName)+1+strlen(dirEnt->d_name));
                        strcpy(pathName,arg->dirName);
                        strcat(pathName,"/");
                        strcat(pathName,dirEnt->d_name);
                        //printf("processing [%s]\n",pathName);
                        // check if the directory is valid
                        if (stat (pathName, &path_stat) != 0) {
                                return ((void *) 1);
                        }
                        else {
                                //if entry is a directory,
                                if (S_ISDIR(path_stat.st_mode)) {
                                        //dirArgs = (struct dirHandlerArgs *) malloc (sizeof(struct dirHandlerArgs));
                                        //construct thread arguments
                                        dirArgs[threadCount].dirName= pathName;
                                        dirArgs[threadCount].shm = arg->shm;
                                        dirArgs[threadCount].mymutex = arg->mymutex;
                                        //create a thread for dirhandler
                                        //printf("creating dir handler thread for %s\n",dirArgs.dirName);

                                        pthread_create( &thread[threadCount], &threadAttrs, dirHandler, (void *) &dirArgs[threadCount]);
                                        threadCount++;
                                }
                                //if entry is a file
                                else if (S_ISREG(path_stat.st_mode)) {
                                        //fileArgs = (struct fileHandlerArgs *) malloc (sizeof(struct fileHandlerArgs));
                                        //construct thread arguments
                                        fileArgs[threadCount].fileName= pathName;
                                        fileArgs[threadCount].shm = arg->shm;
                                        fileArgs[threadCount].mymutex = arg->mymutex;
                                        //create a thread for filehandler
                                        //printf("creating file handler thread for %s\n",fileArgs.fileName);

                                        pthread_create( &thread[threadCount], &threadAttrs, fileHandler, (void *) &fileArgs[threadCount]);
                                        threadCount++;
                                }
                                //if anything else ignore and continue
                                else
                                        continue;
                        }
                }

                //join
                for (int i =0 ; i < threadCount; i++) {
                        //printf("dirHandler:Joining  %d:%d\n", localId,i);
                        pthread_join(thread[i], NULL);
                }
                pthread_attr_destroy(&threadAttrs);
        }
        if (strcmp(arg->dirName,topdirName) == 0) { // for the direct  call from main
                return 0;

        }
        else {
                pthread_exit((void*) 0); // for thread invocation
        }
}

/*
 * Main function
 * takes in a directory name as input
 * Post validation, calls the directory handler
 * On getting control back ( directory has been explored and
 * all files tokenized) , analyses it using the provided algorithm
 * for similarity
 * Finally prints output
 */

int main (int argc, char **argv){

        pthread_mutex_t *mymutex;
        struct shareMemStruct *shm;
        struct dirHandlerArgs *dirArgs;
        struct stat path_stat;
        char *dirName;

        /* test code to test tokenizer
        outloop = (struct fileNode *) malloc (sizeof(struct fileNode ));
        outloop->fileName = argv[1];
        outloop->firstToken = NULL;
        outloop->nextFile = NULL;
        tokenizer(outloop);
        dumpfileNode(outloop);                                 
        return  1;
        */

        if (argc != 2) {
                fprintf(stderr,"Invalid usage!\n");
                fprintf(stderr,"Usage : %s <dirname>\n",argv[0]);
                return 1;
        }

        // save first arg
        topdirName = (char *) malloc(strlen(argv[1]));
        strcpy(topdirName, argv[1]);
        dirName = (char *) malloc(strlen(argv[1]));
        strcpy(dirName, argv[1]);
        // check if the directory is valid
        if (stat (dirName, &path_stat) != 0) {
                fprintf(stderr,"Invalid argument\n");
                fprintf(stderr,"Usage : %s <dirname>\n",argv[0]);
        }
        else {
                if (!S_ISDIR(path_stat.st_mode)) {
                        fprintf(stderr,"%s is Not a directory\n", dirName);
                        fprintf(stderr,"Usage : %s <dirname>\n",argv[0]);
                        return 1;
                }
                if (access(dirName,R_OK | X_OK) != 0) {
                        printf("directory %s is not accessible \n", dirName);
                        printf("Usage : %s <dirname>\n",argv[0]);
                        return 1;
                }
        }

        // initialize mutex for global use. allocate on heap for access
        mymutex  = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(mymutex, NULL);

        //allocate shared memory
        shm  = (struct shareMemStruct *) malloc(sizeof(struct shareMemStruct));
        shm->firstFileNode = NULL;
        dirArgs = (struct dirHandlerArgs *) malloc (sizeof(struct dirHandlerArgs));

        //create dirArgs
        dirArgs->dirName= dirName;
        dirArgs->shm = shm;
        dirArgs->mymutex = mymutex;

        //printf("Dir = %s\n", dirArgs->dirName);
        dirHandler(dirArgs);
        //dumpSHM(shm); // dumo the Shared Global memory

        pthread_mutex_destroy(mymutex); // done with this
        free(dirName);   // done with this , so free up
        free(dirArgs);   // done with this , so free up
        free(mymutex);   // done with this , so free up


        //call file compare and print results
        //printf("Starting file analysis\n");
        struct fileNode *first;
        struct fileNode *second;
        struct JSD *currentJSD , *jsdn , *allJSD;
        double score = 0;
        allJSD = NULL;
        first = shm->firstFileNode;
        while (first != NULL) {
                second = first;
                while(second != NULL) {
                        if (strcmp(first->fileName, second->fileName)) {
                                score = fileCompComputeJSD(first,second);
                                if (allJSD == NULL) { // if no JSD yet
                                        allJSD = (struct JSD *) malloc (sizeof(struct JSD));
                                        allJSD->first = first;
                                        allJSD->second = second;
                                        allJSD->totalTokenCount = first->totalTokensInFile + second->totalTokensInFile;
                                        allJSD->score = score;
                                }
                                else if ((first->totalTokensInFile + second->totalTokensInFile) <= allJSD->totalTokenCount) {// insert at top of the list
                                        jsdn = (struct JSD *) malloc (sizeof(struct JSD));
                                        jsdn->next = allJSD;
                                        jsdn->first = first;
                                        jsdn->second = second;
                                        jsdn->totalTokenCount = first->totalTokensInFile + second->totalTokensInFile;
                                        jsdn->score = score;
                                        allJSD = jsdn;
                                }
                                else {
                                        currentJSD = allJSD;
                                        while (1) {
                                                // if current link total is less than new total
                                                if (currentJSD->totalTokenCount <= (first->totalTokensInFile + second->totalTokensInFile) )   {
                                                        // insert a link
                                                        jsdn = (struct JSD *) malloc (sizeof(struct JSD));
                                                        jsdn->next = currentJSD->next;
                                                        currentJSD->next = jsdn;
                                                        currentJSD = currentJSD->next;
                                                        currentJSD->first = first;
                                                        currentJSD->second = second;
                                                        currentJSD->totalTokenCount = first->totalTokensInFile + second->totalTokensInFile;
                                                        currentJSD->score = score;
                                                        break;
                                                }
                                                else {
                                                        // check if next link is NULL (last link)
                                                        if (currentJSD->next == NULL) { // end of the list , add a link
                                                                currentJSD->next = (struct JSD *) malloc (sizeof(struct JSD));
                                                                currentJSD = currentJSD->next;
                                                                currentJSD->first = first;
                                                                currentJSD->second = second;
                                                                currentJSD->totalTokenCount = first->totalTokensInFile + second->totalTokensInFile;
                                                                currentJSD->score = score;
                                                                break;
                                                        }
                                                        else
                                                                // set current link as the next link
                                                                currentJSD = currentJSD->next;
                                                }
                                        }
                                }
                        }
                        second = second->nextFile;
                }
                first = first->nextFile;

        }
        printJSD(allJSD);
        //printf("Free shared memeory\n");
        freeSHM(shm);    // done with this , so free up


        return 0;
}
