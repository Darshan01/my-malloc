AUTHORS: Darshan Lakshminarayanan
NetIds: DL1058

---------------------------------------------PROJECT/FILE DESCRIPTIONS---------------------------------------------

Custom, toy versions of malloc() and free() made for Rutgers - New Brunswick CS 214, Spring 2024

mymalloc.c, mymalloc.h
    contain implementations of malloc() and free()

memtest.c
    provided by instructor
    allocates memory with malloc, writes to that memory, and detects if the memory was written correctly
    prints number of incorrect bytes in memory

memgrind.c
    contains 5 stress test for our implementation of malloc, 3 given by the instructor and 2 custom
    prints the average time it takes for each test
    prints the amount of leaky memory
    tests and their average time performance are described below in the PERFORMANCE section

errortest.c
    demonstrates the error detection capabilities of our implementation
    error are described below in the ERRORS section

Makefile
    contains compilation instructions

---------------------------------------------AVAILABLE FUNCTIONS---------------------------------------------

void *mymalloc(size_t size, char *file, int line);
    allocates the number of bytes given by size_t size to memory
    uses char *file and int line to provide a location for any errors it detects

void myfree(void *ptr, char *file, int line);
    frees the pointer given by void *ptr
    uses char *file and int line to provide a location for any errors it detects

int leakyMemory();
    can be called at the end of runtime
    returns the number of pointers in the heap that were malloc-ed but not yet freed

---------------------------------------------RUNNING INSTRUCTIONS---------------------------------------------

make sure you are in the directory with all the files described in the PROJECT/FILE DESCRIPTIONS section
enter the following into the command line:
    make

below is a description of the files created:

test:
    runs memtest.c with our malloc implementation

realTest:
    runs memtest.c with the real malloc implementation

memgrind:
    runs memgrind.c with our malloc implementation

realGrind:
    runs memgrind.c with the real malloc implementation

errortest:
    runs errortest.c with our malloc implementation

realError:
    runs errortest.c with the real malloc implementation

after running make, to run any of these files, type:
    ./FILENAME

and replace filename with any of the files described above, ie. ./memgrind


---------------------------------------------PERFORMANCE TESTS---------------------------------------------

All tests are run 50 times each and the average run time is recorded for both real malloc and our malloc

TEST 1 (provided by instructor)
    malloc and free a 1 byte object, repeat 120 times

    our malloc average run time: 3 microseconds
    real malloc average run time: 25 microseconds

TEST 2 (provided by instructor)
    first malloc 120 1 byte objects, then free all of them

    our malloc average run time: 60 microseconds
    real malloc average run time: 25 microseconds

TEST 3 (provided by instructor)
    create an array of 120 pointers, then randomly choose between 2 choice:
        1. allocate 1 byte and store the pointer in the array
        2. free the most recently allocated pointer, if any
    continue until 120 allocations have been made, then free all remaining objects

    our malloc average run time: 5545 microseconds
    real malloc average run time: 3133 microseconds

CUSTOM TEST 1
    first, allocate 120 8 byte objects
    then, free 2 objects in a row, then skip one, such that our heap has this kind of format:
        [free, free, allocated, free, free, allocated ...]
    next, allocate 40 16 byte objects
        this step checks that when there are 2 free blocks in a row, they are coalesced
    finally, free all the objects

    our malloc average run time: 126 microseconds
    real malloc average run time: 40 microseconds

CUSTOM TEST 2
    first, allocate 120 8 byte objects
    then, free every other object, such that the heap has this kind of format:
        [free, allocated, free, allocated ...]
    next, free the remaining objects
        this ensures that when there are 3 free blocks in a row, they are coalesced
    finally, allocate 4088 bytes and then free it
        this ensures that the memory was properly freed

    our malloc average run time: 116 microseconds
    real malloc average run time: 17 microseconds

Note: these performance times are for Rutgers University iLab machines; performances may vary on different machines

---------------------------------------------ERRORS---------------------------------------------

this section describes all the error messages and their possible causes

malloc: cannot allocate 0 bytes of data
    the user called malloc(0)

malloc: tried to malloc x bytes; greater than size of heap
    the user called malloc(x) but x is greater than the number of bytes in the entire heap
    no matter how much is freed, this call will never work

malloc: tried to malloc x byte(s) of data; not enough space in heap; consider freeing data to make space
    the user called malloc(x), but there is not enough space for x bytes in the heap
    this could be fixed if enough malloc-ed pointers are freed

note: malloc returns NULL for any of the above errors

free: tried to free NULL pointer
    the user tried to call free(ptr) but ptr was NULL

free: tried to free a pointer that was not malloc-ed; pointer not in heap, not obtained from malloc
    the user called free(ptr) but ptr points to somewhere not in the heap, so it could not have been obtained from malloc
    an example of this is shown in errortest.c line 29

free: tried to free a pointer that was not malloc-ed
    the user called free(ptr) but malloc has not been called yet, so ptr could not have been obtained from malloc

free: tried to free the same pointer multiple times
    the user called free(ptr) but the ptr was already freed
    an example of this is shown in errortest.c lines 19-24
    note: sometimes it is not possible to determine if a pointer was double freed, as described below

free: tried to free a pointer that was not malloc-ed; pointer not in heap, not obtained from malloc
    the user called free(ptr) but the ptr is either:
        pointing to a random location in the heap that does not correspond to a malloc-ed chunk
        or pointing to a location that was already freed

    there is some ambiguity in these cases that cannot be detected by our malloc implementation
    the ambiguity is highlighted in errortest.c lines 31-43
        in these lines we see that freeing an offset pointer and double freeing results in the same error
        this is due to the fact that once the free blocks are coalesced, it is impossible to tell what was previously inside those free blocks

---------------------------------------------WHAT I LEARNED---------------------------------------------
I learned how to create multi-file projects in C
I learned how to create files from scratch
I learned how to collaborate on a coding project
I learned how to debug large projects
