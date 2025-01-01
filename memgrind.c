#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
// Compile with -DREALMALLOC to use the real malloc() instead of mymalloc()
#ifndef REALMALLOC
#define REALMALLOC 0
#include "mymalloc.h"
#endif
#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 4
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)


int main(int argc, char **argv)
{   
    struct timeval starttime;
    struct timeval endtime;
    
    //TEST 1
    int* ptr;
    gettimeofday(&starttime, NULL);
    
    for(int n = 0; n < 50; n++){
        for(int i = 0; i < 120; i++){
            ptr = malloc(1);
            if(ptr == NULL){
                printf("Failed");
                return EXIT_FAILURE;
            }
            free(ptr);
        }
    }

    gettimeofday(&endtime, NULL);
    printf("Test 1 took an average of %li seconds and %lo microseconds\n", (endtime.tv_sec - starttime.tv_sec)/50, (endtime.tv_usec-starttime.tv_usec)/50);

    //TEST 2
    char* arr[120];
    gettimeofday(&starttime, NULL);

    for(int n = 0; n < 50; n++){
        for(int i = 0; i < 120; i++){
        arr[i] = (char*) malloc(1);
            if(arr[i] == NULL){
                printf("Failed");
                return EXIT_FAILURE;
            }
        }
        for(int i = 0; i < 120; i++){
            free(arr[i]);
        }
    }

    gettimeofday(&endtime, NULL);
    printf("Test 2 took an average of %li seconds and %lo microseconds\n", (endtime.tv_sec - starttime.tv_sec)/50, (endtime.tv_usec-starttime.tv_usec)/50);

    //TEST 3
    char* ran[120];
    int allocated;
    gettimeofday(&starttime, NULL);

    for(int n = 0; n < 50; n++){
        allocated = 0;
        while(allocated < 120){
            int l = rand();
            if(l%10 <= 4){
                ran[allocated] = (char*) malloc(1);
                allocated++;
            } else {
                if(allocated != 0){
                    free(ran[allocated-1]);
                    allocated--;
                } else {
                    //nothing is allocated, do nothing
                }
                
            }
        }
        for(int i = 0; i < allocated; i++){
            free(ran[i]);
        }
    }

    gettimeofday(&endtime, NULL);
    printf("Test 3 took an average of %li seconds and %lo microseconds\n", (endtime.tv_sec - starttime.tv_sec)/50, (endtime.tv_usec-starttime.tv_usec)/50);

    //CUSTOM TEST 1: Described in README
    char* obj[120];
    int i;
    int j;
    gettimeofday(&starttime, NULL);

    for(int n = 0; n < 50; n++){

        for(i = 0; i < 120; i++){
            obj[i] = malloc(8);
        }

        i = 0;
        while(i < 120){
            free(obj[i]);
            free(obj[i+1]);
            i+=3;
        }

        i = 0;
        while(i < 120){
            obj[i] = malloc(16);
            i+=3;
        }

        i = 0;
        j = 2;
        while(j < 120 && i < 120){
            free(obj[i]);
            free(obj[j]);
            i+=3;
            j+=3;
        }

    }

    gettimeofday(&endtime, NULL);
    printf("Custom Test 1 took an average of %li seconds and %lo microseconds\n", (endtime.tv_sec - starttime.tv_sec)/50, (endtime.tv_usec-starttime.tv_usec)/50);

    //CUSTOM TEST 2: Described in README
    char* obj1[120];
    gettimeofday(&starttime, NULL);
    
    for(int n = 0; n < 50; n++){
        for(i = 0; i < 120; i++){
            obj1[i] = malloc(1);
        }
        
        for(i = 0; i < 120; i+=2){
            free(obj1[i]);
        }

        for(i = 1; i < 120; i+=2){
            free(obj1[i]);
        }
        int* full = malloc(MEMSIZE - HEADERSIZE);
        free(full);
    
    }

    gettimeofday(&endtime, NULL);
    printf("Custom Test 2 took an average of %li seconds and %lo microseconds\n", (endtime.tv_sec - starttime.tv_sec)/50, (endtime.tv_usec-starttime.tv_usec)/50);

    if(!REALMALLOC){
        int leak = leakyMemory();
        if(leak == 0){
            printf("\nno leaky memory\n"); //all malloc-ed pointers were freed
        } else {
            fprintf(stderr, "\nleaky memory: %i malloc-ed pointers have not been freed\n", leakyMemory()); //print how many malloc-ed pointers have not been freed.
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;

}
