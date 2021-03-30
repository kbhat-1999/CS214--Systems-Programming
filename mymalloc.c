#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"
int memInit = 0;
char mymem[MEMSIZE];

/*
convertMetadataToBitMap is a function that takes the size and
the inUse and converts it to a bitmap. It sets the inUse to 0 or 1,
depending on the value of the inUse. Both the size and inUse will
be stored in individual metadata blocks, each of which is located
before each group of allocated memory, as well as the remaining free
memory. "size" represents the amount of memory the user wants allocated,
 which must be less than 4096 bytes. inUse states whether a set of memory
is currently in used or if it has been freed. 2 bytes are allocated for
each metadata block in the memory array. "first" represents the first
byte of the metadata, and it will hold the bit for inUse. It will also
hold up to 4 bits for "size", if the inputted size is more than 128 bytes.
"second" represents the second byte of the metadata block, and it will
hold bits for size values less than 128 bytes. 
*/   

void convertMetadataToBitMap (unsigned int size, unsigned int inUse, char* first, char* second) {

    char szArr[17]; // 2 bytes are allocated for metadata blocks. 2 bytes = 16 bits
    int i = 16; 
    szArr[i--]='\0';
    while (size > 0) {
        szArr[i--] = (char) size % 2 + '0';
        size = size / 2;
    }
    while(i>=0)
      szArr[i--]='0';

    //First element of the bitmap represents inUse. if inUse = 1, flip the szArr[0] of the bitmap to '1'. Otherwise, keep szArr[0] to '0'   
    if (inUse == IN_USE)
        szArr[0] = '1';

    //printf("convertMetadataToBitMap:szArr = %s\n",szArr);
    *second  = (unsigned char) strtoul(szArr+8, 0, 2);
    szArr[8] = '\0';
    *first = (unsigned char) strtoul(szArr, 0, 2);

}


/*
deconvertMetadataFromBitMap is a function that extracts the inUse bit
from "first", as well as the last 4 bits from "first". The last 4 bits
in "first" represent size values that are greater than 128. These will
be coalesced with the bits stored in "second"
*/

void deconvertMetadataFromBitMap (char first, char second, unsigned int* size, unsigned int* inUse) {
    char szArr[17];
    szArr[16]='\0';

    // printf("deconvertMetadataFromBitMap:%u %u %c %c\n", first, second, first, second);

    /*
    Each bit in "second" will be "and"-ed with 1 in order to maintain the value
    stored in this array. This value will be coalesced with the last 4 bits of
    "first" to form an array of size 13 (the first 12 elements of the array
    represents the size the user allocates. Because the maximum amount of bytes
    that can be allocated is 4096 bytes, log2(4096) = 12 bits)
    */

    for(int n = 0; n < 8; n++) {
        szArr[15-n]  = ((second >> n) & 1) + '0' ;
        // printf("deconvertMetadataFromBitMap:szArr[%d] = %c\n",15-n,szArr[15-n]);
    }

    //Extracts the bit for inUse and the remaining 4 bits used for size. 
    for(int n = 0; n < 8; n++) {
        szArr[7-n]  = ((first >> n) & 1) + '0' ;
        // printf("deconvertMetadataFromBitMap:szArr[%d] = %c\n",7-n,szArr[7-n]);
    }

    // printf("deconvertMetadataFromBitMap:szArr = %s\n",szArr);


    if (szArr[0] == '1')
        *inUse = IN_USE;
    else
        *inUse = NOT_IN_USE;
    //szArr[0] is flipped back to 0  to calculate size
    szArr[0] = '0';
    *size  = (unsigned int) strtoul(szArr, 0, 2);

}


/*
coalesceFreeMemory will coalsece both the first and second groups of
free space when free() is called. It will also add 2 bytes, as this is
the size of the metadata block that was initially between both groups
of free space. otherFirst represents the first byte of the metadata block
for  next group of memory. otherSecond represents the second byte of
the metadata block for the next group of memory
*/

void coalesceFreeMemory (char* first, char* second, char* otherFirst, char* otherSecond) {
    unsigned int size;
    unsigned int inUse;
    unsigned int otherSize; //size of the next group of memory, whether the memory is alread allocated or if it is free memory
    unsigned int otherInUse; //states whether a given group of memory is in use or if it is free memory

    //deconvert the bits stored in the bitmap back to "size" and "inUse" for both group of memory.
    deconvertMetadataFromBitMap (*first, *second, &size, &inUse);
    deconvertMetadataFromBitMap (*otherFirst, *otherSecond, &otherSize, &otherInUse);

    // adding the deconverted sizes from both bit maps and 2 bytes (size of the metadata block
    // in between both groups of memory) to coalesce the memory after free() has been called.
    if (inUse == NOT_IN_USE && otherInUse == NOT_IN_USE)
        size = size+otherSize+2;

    //memory has been coalesced. must change the values stored in the first metadata block to reflect this change.
    convertMetadataToBitMap (size, NOT_IN_USE, first, second);


}


void* mymalloc(size_t sz, const char* file, int line) {
    int index;
    unsigned int size;
    unsigned int inUse;
    unsigned int sizeLeft;
    unsigned int offsetFromCurrent;
    unsigned int newSize;
    char *data;
    unsigned int sizeToAllocate;
    int chkSize;

    index = 0;
    data = mymem;
    sizeToAllocate = (unsigned int) sz;
    chkSize  = sz;

    /* if first invocation , set meata in first two cells */
    if (!memInit) {
        memInit = 1;
        convertMetadataToBitMap (MEMSIZE, NOT_IN_USE, data, (data+1));
        deconvertMetadataFromBitMap (*data, *(data+1), &size, &inUse);
        //printf("mymalloc1: size = %d inuse = %u \n", size, inUse);
    }
    if (chkSize < 0) {
        //printf("ERR:%s:%d Cannot allocate negative memory \n", file,line);
        return NULL;
    }
    if (sizeToAllocate > (MEMSIZE-METASIZE)) {
        //printf("ERR:%s:%d Cannot allocate %d . More than maximum memory \n", file,line,sizeToAllocate);
        return NULL;
    }
    if (sizeToAllocate == 0) {
        //printf("ERR:%s:%d Cannot allocate zero size memory %d \n", file,line,sizeToAllocate);
        return NULL;
    }
    while (1) {
        deconvertMetadataFromBitMap (*data, *(data+1), &size, &inUse);
        //printf("mymalloc2: size = %d inuse = %u \n", size, inUse);
        // if free split it and allocate what we need
        if (inUse == NOT_IN_USE) {
            if (size > sizeToAllocate) {
                if (size >(sizeToAllocate+2) )
                    splitFreeMemory (data,  data+1, sizeToAllocate, &newSize, &sizeLeft, &offsetFromCurrent ) ;
                //printf("mymalloc3: sizeToAllocate=%u newSize=%u sizeLeft=%u offsetFromCurrent=%u \n",sizeToAllocate,newSize, sizeLeft, offsetFromCurrent);
                convertMetadataToBitMap (sizeToAllocate, IN_USE, data, data+1);

                //printf("mymalloc4: size = %d inuse = %u \n", size, inUse);
                if (! ( data+offsetFromCurrent > mymem+MEMSIZE || data+offsetFromCurrent+1 > mymem+MEMSIZE)  )
                    convertMetadataToBitMap (sizeLeft, NOT_IN_USE, data+offsetFromCurrent, data+offsetFromCurrent+1);
                //printf("SUC:%s:%d  allocated %d at %p \n", file,line,sizeToAllocate,data+2);
                return (data+2);
            }
            else { // do one Coalesce  ...can be a loop
                coalesceFreeMemory (data, data+1, data+METASIZE+size,data+METASIZE+size+1 ) ;
                deconvertMetadataFromBitMap (*data, *(data+1), &size, &inUse);
                if (size > sizeToAllocate) {
                    if (size >(sizeToAllocate+2) )
                        splitFreeMemory (data,  data+1, sizeToAllocate, &newSize, &sizeLeft, &offsetFromCurrent ) ;
                    convertMetadataToBitMap (sizeToAllocate, IN_USE, data, data+1);
                //printf("mymalloc5: size = %d inuse = %u \n", size, inUse);
                    if (! ( data+offsetFromCurrent > mymem+MEMSIZE || data+offsetFromCurrent+1 > mymem+MEMSIZE)  )
                        convertMetadataToBitMap (sizeLeft, NOT_IN_USE, data+offsetFromCurrent, data+offsetFromCurrent+1);
                    //printf("SUC:%s:%d  allocated %d at %p \n", file,line,sizeToAllocate,data+2);
                    return (data+2);
                }
                else  {
                    //printf("ERR:%s:%d Cannot allocate %d . free mem not found\n", file,line,sizeToAllocate);
                    return NULL;
                }
            }
        }
        else { //data is not free, must continue searching for next set of contigous data
            index = index+2+size;
            if (index >= 4096) { //maximum size of the array is 4096 bytes. unable to continue searching contigous data
                //printf("ERR:%s:%d Cannot allocate %d . free mem not found\n", file,line,sizeToAllocate);
                return NULL;
            }
            else {
                data = data+2+size;
                continue;
            }
        }
    }
}

void myfree(void *ptr, const char* file, int line) {
    unsigned int size;
    unsigned int inUse;
    char *data;

   if (! (ptr >= (void *) (mymem+METASIZE) && ptr < (void *) (mymem+MEMSIZE))) {
       //printf("ERR:%s:%d %p is not a pointer or not a valid malloc allocated ptr \n", file,line,ptr);
       return;
   }

   data = mymem;
   while(1) {
       deconvertMetadataFromBitMap (*data, *(data+1), &size, &inUse);
       //printf("myfree1:size=%u inUse=%u\n", size, inUse);
       if (ptr == (data+2)) {
           if (inUse == IN_USE) {
               // free this mem by setting inUse to NOT_IN_USE
               convertMetadataToBitMap (size, NOT_IN_USE, data, data+1);
               //printf("SUC:%s:%d Freed memory of size %u at  %p \n", file,line,size ,ptr);
               return;
           }
           else {
                //printf("ERR:%s:%d Memory %p not in Use\n", file,line,ptr);
                return;
           }
       }
       else {
           data = data+2+size;
           if (! (data > mymem+METASIZE && data < mymem+4096)) {
               //printf("ERR:%s:%d %p is not a pointer or not a valid malloc allocated ptr \n", file,line,ptr);
               return;
           }
       }

   }

}

/*
splitFreeMemory seperates memory in use from free memory using metadata blocks.
The argument "sizeToAllocate" is the size of the next group of memory that needs
to be allocated. "* sizeLeft" represents the amount of free memory that is
remaining after a new set of memory is allocated. "* offsetFromCurrent" refers
to the amount of memory that is being used for the new allocation, which involves
the size of the metadata block and the size of the input 
*/

void splitFreeMemory (char* first, char* second, unsigned int sizeToAllocate, unsigned int *newSize,unsigned int *sizeLeft, unsigned int *offsetFromCurrent) {
    unsigned int size;
    unsigned int inUse;

    deconvertMetadataFromBitMap (*first, *second, &size, &inUse);
    //printf("splitFreeMemory1: size = %d inuse = %u \n", size, inUse);

    *sizeLeft = size-sizeToAllocate-2; //must subtract the input size and the size of the metadata block from 4096
    *offsetFromCurrent = 2+sizeToAllocate;
    *newSize = size-sizeToAllocate;
    // Add in check if the spze is just a little bit more , return in that case

}
