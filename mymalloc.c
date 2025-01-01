#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"
#ifndef DEBUG
#define DEBUG 0
#endif

#define MEMLENGTH 512
#define ALIGN 8 //value to round to for alignment

#define ALLOCATED 2 //value to set when a block is allocated
#define FREE 0 //value to set when a block is free
#define FREED 1 //value to set when a block was allocated but is now freed

static double mem[MEMLENGTH];

int nearestMult(size_t, int);
char* findFirstFree(void*, int);
int validate(void*);
void coalesce();

void *mymalloc(size_t size, char *file, int line){
    //return an error if the user tries to allocate 0 bytes
    if(size == 0){
        fprintf(stderr, "malloc: cannot allocate 0 bytes of data (%s:%i)\n", file, line);
        return NULL;
    }

    //round size of payload up to 8
    int loadSize = nearestMult(size, ALIGN);

    //if the payload size is greater than the available size of the heap, return an error
    if(loadSize > MEMLENGTH*sizeof(double)-2*sizeof(int)){
        fprintf(stderr, "malloc: tried to malloc %zu bytes; greater than size of heap (%s:%i)\n", size, file, line);
        return NULL;
    }

    //find the first free block with enough space for our payload
    //**pointer points to metadata**
    int* insert = (int*) findFirstFree(mem, loadSize);

    /*  findFirstFree returns null if a free block with enough space was not found
    return an error */
    if(insert == NULL){
        if(size == 1){
            fprintf(stderr, "malloc: tried to malloc %zu byte of data; not enough space in heap; consider freeing data to make space (%s:%i)\n", size, file, line);
            return NULL;
        }
        fprintf(stderr, "malloc: tried to malloc %zu bytes of data; not enough space in heap; consider freeing data to make space (%s:%i)\n", size, file, line);
        return NULL;
    }

    //set free block as allocated
    *insert = ALLOCATED;

    if(DEBUG) printf("LoadSize: %i\n", *(insert+1));

    /*  create hold pointer to hold the place of the free block
    after our allocated block */ 
    char* hold;
    
    //if the heap is unitizalized
    if(*(insert+1) == 0){
        hold = ((char*) insert) + (2*sizeof(int) + loadSize);
        if(DEBUG) printf("Size of Free: %i\n", *(((int*) hold) + 1));

        //if the free block is still within the bounds of our heap
        if(hold < ((char*) (mem + MEMLENGTH))){
            *((int*) hold) = FREE; //mark that block as free
            *(((int*) hold) + 1) = MEMLENGTH*sizeof(double) - (loadSize + 4*sizeof(int)); //set size of free block as the size of the heap minus the payload size and two metadata blocks
        }

        *(insert+1) = loadSize; //set the payload size of our allocated block
    } 

    //if the heap is initialized
    else {
        int initialSize = *(insert+1); //store the initial size of the free block

        //if the amount allocated is not equal to the size of the free block
        if(loadSize-initialSize !=0){
            hold = ((char*) insert) + (2*sizeof(int) + loadSize);
            if(DEBUG) printf("Size of Free: %i\n", *(((int*) hold) + 1));
            *((int*) hold) = FREE; //mark that block as free
            *(((int*) hold) + 1) = initialSize - (loadSize+2*sizeof(int)); //set size of new free block
        }

        *(insert+1) = loadSize; //set payload size of allocated block
    }
    if(DEBUG){
        printf("Top of Heap: %p\n", mem);
        printf("Pointer: %p\n", insert);
        printf("Pointer to free: %p\n",hold);
        printf("End of heap: %p\n\n", ((char*) (mem + MEMLENGTH)));
    }
    
    return (void*) (insert+2); //return pointer to payload (8 bytes after metadata)

}

void myfree(void *ptr, char *file, int line){
    if(ptr == NULL){
        fprintf(stderr, "free: tried to free NULL pointer (%s:%i)\n", file,line);
        return;
    }

    //validate pointer; errors described in README
    switch(validate(ptr)){
        case 0:
            break; //pointer is valid
        case 1:
            fprintf(stderr, "free: either tried to free a pointer that was not malloc-ed or free the same pointer multiple times (%s:%i)\n", file,line);
            return;
        case 2:
            fprintf(stderr, "free: tried to free the same pointer multiple times (%s:%i)\n", file,line);
            return;
        case 3:
            fprintf(stderr, "free: tried to free a pointer that was not malloc-ed; pointer not in heap, not obtained from malloc (%s:%i)\n", file, line);
            return;
        case 4:
            fprintf(stderr, "free: tried to free a pointer that was not malloc-ed (%s:%i)\n", file, line);
            return;
    }

    *(((int*) ptr) - 2) = FREED; //mark block as freed
    coalesce(); //combine adjacent free blocks
}

/****malloc helper functions****/

//round up to the alignment size (8)
int nearestMult(size_t size, int alignment){
    return (size + (alignment - 1)) & ~(alignment - 1);
}

//finds the first free block starting at the "start" void pointer with size greater than or equal to length
char* findFirstFree(void* start, int length){
    int* ptr = (int*) start;

    //for uninitialized data
    if(*(ptr+1)==0 && (char*) ptr == (char*) mem){
       return (char*) ptr;
    }

    //while the block is in the heap, allocated or too small, keep looking
    while((char*) ptr < ((char*) (mem + MEMLENGTH)) && (*ptr == ALLOCATED || *(ptr+1) < length)){

        ptr = (int*) (((char*)ptr) + (*(ptr+1) + 2*sizeof(int))); //go to next block
    
    }

    //free block was not found
    if((char*) ptr >= ((char*) (mem + MEMLENGTH))){
        return NULL;
    }

    return (char*) ptr; //free block was found
}

/****free helper functions****/

//returns a value indicating validity of pointer passed to free
int validate(void* p){
    //the pointer is not in the range of our heap
    if((char*) p >= ((char*) (mem + MEMLENGTH)) || (char*) p < ((char*) mem) ){
        return 3;
    }

    int* ptr = (int*) mem;

    //heap is unitialized
    if(*(ptr+1)==0) return 4;

    //iterate through the heap
    while((char*) ptr < ((char*) (mem + MEMLENGTH))){

        if(((char*) ptr)+2*sizeof(int) == (char*) p && *ptr == ALLOCATED){
            return 0; //good, pointer can be freed

        } else if(((char*) ptr)+2*sizeof(int) == (char*) p && *ptr == FREED){
            return 2; //pointer was recently freed

        }

        ptr = (int*) (((char*)ptr) + (*(ptr+1) + 2*sizeof(int))); //go to next block in heap
    }

    return 1; //pointer was either already freed or not malloc-ed
}

//combines adjacent free blocks
void coalesce(){
    int* ptr = (int*) mem;

    //loop through memory until we reach end of heap
    while((char*) ptr < ((char*) (mem + MEMLENGTH))){
        
        //find first free block with any size, hence second argument is 0
        ptr = (int*) findFirstFree(ptr, 0);

        if(ptr == NULL){return;}//no free blocks found, end function

        int sum = 0;
        int* temp = ptr; //create another pointer to the free block

        //iterate through the heap until we find an allocated block
        while((((char*) ptr) < ((char*) (mem + MEMLENGTH))) && *ptr < ALLOCATED){

            sum += (*(ptr+1)+2*sizeof(int)); //add the size of the each free block to our sum
            ptr = (int*) (((char*)ptr) + (*(ptr+1) + 2*sizeof(int))); //go to the next block
        
        }

        *(temp+1) = sum - 2*sizeof(int); //set the size of the first free block to hold the successive free blocks

    }
    return;
}

/*****leaky memory*****/
//user can call at the end of runtime to determine if any malloc-ed pointers were not freed
int leakyMemory(){
    int* ptr = (int*) mem;

    //uninitialized data
    if(*(ptr+1)==0){
       return 0;
    }

    int numLeaky = 0; // count the number of pointers not freed

    while((char*) ptr < ((char*) (mem + MEMLENGTH))){
        numLeaky += (*ptr == ALLOCATED);
        ptr = (int*) (((char*)ptr) + (*(ptr+1) + 2*sizeof(int)));
    }

    return numLeaky;
}