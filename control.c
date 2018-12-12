#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/ipc.h>

int main(int argc, char * argv[]){

  if (argc == 0){
    printf("You did not enter any arguments.");
    exit(0);
  }
  char * argument = argv[0];
  printf("%s",argument);

  return 0;
}
