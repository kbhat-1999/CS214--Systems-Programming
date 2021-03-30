#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"
#include <time.h>


void basic_tests()
{
        char *ptr = NULL;
        //char *ptr2 =NULL;

        free(ptr);
        // allocate more than available
        ptr = (char *) malloc(5088);
        // allocate nothing
        ptr = (char *) malloc(0);
        // allocate negatove
        ptr = (char *) malloc(-1);

        // free using a integer
        free (0);

        // malloc , free and then free again
        ptr = malloc(1);
        free (ptr);
        free (ptr);

        // malloc , but free  some other part of the memory
        ptr = malloc(1);
        free (ptr+1);


        //
}

/*
 * workload 1
 *
 */

void load1() {
        char *ptr;
        int maxrun = 120;
        for (int i = 0 ; i < maxrun; i++) {
                ptr = (char *) malloc(1);
                free (ptr);
        }
}

/*
 * workload 2
 *
 */

void load2() {
        char *ptr[120];
        int maxrun = 120;
        for (int i = 0 ; i < maxrun; i++) {
                ptr[i] = (char *) malloc(1);
        }
        for (int i = 0 ; i <  maxrun; i++) {
                free(ptr[i]);
        }
}


/*
 * workload 3
 *
 */


void load3() {
        char *ptr[120];
        int maxmalloccount = 120;
        int maxfreecount = 120;
        int i ; 

        /* Intializes random number generator */
        srand(42);

        for (int j =0 ; j<120; j++)
                ptr[j] = NULL;
        while (1) {
                i = rand() % 2;
                if ( i == 0) {  // do a malloc
                        if (maxmalloccount >= 0) {
                                maxmalloccount--;
                                ptr[maxmalloccount]  = (char *) malloc(1);
                        }
                }
                else if (i == 1) { // free a previous
                        if (maxfreecount >= 0) {
                                for (int j =0 ; j < 120; j++) {
                                        if (ptr[j] != NULL)  {
                                                free(ptr[j]);
                                                ptr[j] = NULL;
                                                maxfreecount--;
                                        }
                                }
                        }
                }
                //printf("%d %d \n", maxmalloccount, maxfreecount);
                if ( maxmalloccount <= 0 && maxfreecount <= 0)
                        break;
        }
}

/*
 * Workload 4
 * Does the basic tests about 120 times
 *
 */

void load4() {
        int maxrun = 120;

        for (int i =0 ; i< maxrun; i++) {
                basic_tests();
        }
}

/*
 * workload 5 is similar to 3 but allocates 2 bytes and does about 200 allocations
 */
void load5() {
        char *ptr[200];
        int maxmalloccount = 200;
        int maxfreecount = 200;
        int i ; 

        /* Intializes random number generator */
        srand(42);

        for (int j =0 ; j<200; j++)
                ptr[j] = NULL;
        while (1) {
                i = rand() % 2;
                if ( i == 0) {  // do a malloc
                        if (maxmalloccount >= 0) {
                                maxmalloccount--;
                                ptr[maxmalloccount]  = (char *) malloc(3);
                        }
                }
                else if (i == 1) { // free a previous
                        if (maxfreecount >= 0) {
                                for (int j =0 ; j < 120; j++) {
                                        if (ptr[j] != NULL)  {
                                                free(ptr[j]);
                                                ptr[j] = NULL;
                                                maxfreecount--;
                                        }
                                }
                        }
                }
                //printf("%d %d \n", maxmalloccount, maxfreecount);
                if ( maxmalloccount <= 0 && maxfreecount <= 0)
                        break;
        }
}



int main (int argc, char **argv)
{

        clock_t t;
        double time_taken ;



        // basic_tests();

        t = clock();
        for (int i = 0 ; i < 50 ; i++)
                load1();
        t = clock() - t;
        time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
        printf("load1() took %f seconds to execute \n", time_taken);

        t = clock();
        for (int i = 0 ; i < 50 ; i++)
                load2();
        t = clock() - t;
        time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
        printf("load2() took %f seconds to execute \n", time_taken);

        t = clock();
        for (int i = 0 ; i < 50 ; i++)
                load3();
        t = clock() - t;
        time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
        printf("load3() took %f seconds to execute \n", time_taken);


        t = clock();
        for (int i = 0 ; i < 50 ; i++)
                load4();
        t = clock() - t;
        time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
        printf("load4() took %f seconds to execute \n", time_taken);


        t = clock();
        for (int i = 0 ; i < 50 ; i++)
                // load5();
                t = clock() - t;
        time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
        printf("load5() took %f seconds to execute \n", time_taken);
}
