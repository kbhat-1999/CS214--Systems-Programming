#include <stdlib.h>
#include <stdio.h>

#define malloc(x) mymalloc( x, __FILE__, __LINE__ )
#define free( x ) myfree( x, __FILE__, __LINE__ )
#define MEMSIZE 4096
#define NOT_IN_USE 0
#define IN_USE 1
#define METASIZE 2

void convertMetadataToBitMap (unsigned int size, unsigned int inUse, char* first, char* second);
void deconvertMetadataFromBitMap (char first, char second, unsigned int* size, unsigned int* inUse);
void coalesceFreeMemory (char* first, char* second, char* otherFirst, char* otherSecond);
void* mymalloc(size_t x, const char* file, int line);
void myfree(void *ptr, const char* file, int line);
void splitFreeMemory (char* first, char* second, unsigned int sizeToAllocate, unsigned int *newSize,unsigned int *sizeLeft, unsigned int *offsetFromCurrent);
/*
int memInit = 0;
char mymem[MEMSIZE];
*/
