#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO */
};

int main(int argc, char * argv[]){

  int key = 0x00c0ffee;
  if (argc < 2){
    printf("You did not enter any arguments.\n");
    exit(0);
  }
  char * argument = argv[1];
  if (strcmp(argument,"-c") == 0){
    //Create the shared memory
    shmget(43212,1024, 0644| IPC_CREAT);
    if (errno){
      printf("The shared memory cannot be created for the following reason: %s\n",strerror(errno));
      exit(1);
    }
    printf("The shared memory has been created!\n");

    int semd;
    semd = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);
    if (semd == -1) {
      printf("The semaphore has already been created.\n");
    }
    else {
      union semun us;
      us.val = 1;
      semctl(semd, 0, SETVAL, us);
      printf("The semaphore has been created!\n");
    }

    int fd = open("story.txt", O_CREAT | O_TRUNC, 0644);
    if (fd < 0){
      printf("There has been an error creating the file and this is the reason why: %s\n",strerror(errno));
      exit(1);
    }
    printf("Successfully created the story file!\n");

  }
  else if (strcmp(argument,"-r") == 0){
    int semd = semget(key,1,0);

    printf("Waiting for other users to finish editing...\n");
    while(semctl(semd,0,GETVAL) == 0);

    printf("Removing story...\n");
    semctl(semd,0,IPC_RMID);

    int status = remove("story.txt");
    if (status != 0){
      printf("Error with removing the story file due to the following cause\n(most likely because the story has already been removed): %s.\n",strerror(errno));
    }

    int shmid = shmget(43212,1024, 0644);
    shmctl(shmid, IPC_RMID, NULL);

    printf("Removed story.\n");

  }
  else if (strcmp(argument,"-v") == 0){
    int fd = open("story.txt", O_RDONLY);

    char contents[8000000];
    if (read(fd,contents,8000000) < 0){
      printf("Error with reading the file.\nTry creating the story file first with the '-c' flag.\n\n");
      exit(1);
    }

    printf("\nFULL STORY HERE\n=========================\n%s",contents);
  }
  else {
    printf("Your argument needs to be formatted like this:\nmake setup args=\"-c\"\n");
  }

  return 0;
}
