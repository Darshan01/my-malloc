#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//Compile with -DREALMALLOC to use the real malloc() instead of mymalloc()
#ifndef REALMALLOC
#define REALMALLOC 0
#include "mymalloc.h"
#endif
#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 64
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)

int main(){
  /*ERROR DETECTION*/
  //all errors described in README

  //double free
  printf("\nError detection for double free:\n");
  int* p = malloc(1);
  int* q = p;
  free(p);
  free(q);

  //pointer that is not in our heap, not obtained from malloc
  printf("\nError detection for pointer not obtained from malloc:\n");
  int x;
  free(&x);

  //pointer in our heap but not malloc-ed
  printf("\nError detection for pointer to middle of chunk:\n");
  int* y = malloc(1);
  free(y + 1);

  //double free that cannot be detected
  printf("\nError detection for alternate double free:\n");
  int* s = malloc(1);
  int* t = malloc(1);
  int* u = t;
  free(s);
  free(t);
  free(u); //double free here


  if(!REALMALLOC){
    int leak = leakyMemory();
    if(leak == 0){
      printf("\nno leaky memory\n"); //all malloc-ed pointers were freed
    } else {
      fprintf(stderr, "\nleaky memory: %i malloc-ed pointers have not been freed\n", leakyMemory()); //print how many malloc-ed pointers have not been freed.
    }
  }
  return EXIT_FAILURE;
}