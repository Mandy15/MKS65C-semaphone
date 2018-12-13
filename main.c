#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>

#define KEY 0x00c0ffee
#define SHMID 43212

int main(){
  int sem_id = semget(KEY, 1, 0);
  struct sembuf * buf;
  struct sembuf *buf = malloc(sizeof(struct sembuf));
  buf->sem_num = 0;
  buf->sem_op = -1;
  buf->sem_flg = 0;
  int status = semop(sem_id, buf, 1);

  int shmid = shmget(SHMID, 1024, 0644);
  if (shmid == -1) {
    printf("shmget error: %s\n", strerror(errno));
    return 1;
  }

  int fd = open("story.txt", O_WRONLY | O_APPEND);
  if (fd == -1) {
    printf("Can't open story: %s\n", strerror(errno));
  }

  char story[8000000];
  int status = read(fd, story, 8000000);
  if (status == -1) {
    printf("Can't read story: %s\n", strerror(errno));
    return 1;
  }

  int * data = shmat(shmid, (void*)0, 0);
  int length = data;

  char* last_line = story + lseek(fd, length, SEEK_END);

  printf("The last line in the story:\n%s\n", last_line);

  printf("Enter the next sentence in the story:\n");
  char * input = malloc(1024*sizeof(char));
  fgets(input, 1024, stdin);
  input[1023] = '\n';
  int new_line_length = strlen(input);

  union semun * sem_data = calloc(sizeof(union semun), 1);
  sem_data->val = new_line_length;

  int semctl_status = semctl(sem_id, 0, SETVAL, sem_data);
  if (semctl_status == -1) {
    printf("%s\n", strerror(errno));
  }

  int wr = write(fd, input, new_line_length);
  if (wr == -1) {
    printf("%s\n", strerror(errno));
  }

  return 0;
}
