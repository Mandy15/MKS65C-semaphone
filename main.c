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

union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO */
};

int main(){
  int sem_id = semget(KEY, 1, 0);
  union semun us;
  printf("Waiting for others to finish editing...\n\n");
  while (!semctl(sem_id, 0, GETVAL, us));

  struct sembuf buf;
  buf.sem_num = 0;
  buf.sem_op = -1;
  buf.sem_flg = SEM_UNDO;
  semop(sem_id, &buf, 1);

  int shmid = shmget(SHMID, 1024, 0644);
  if (shmid == -1) {
    printf("shmget error: %s\n", strerror(errno));
    return 1;
  }

  int * data = shmat(shmid, (void*)0, 0);
  int length = *data;

  int fd = open("story.txt", O_RDONLY);
  char story[8000000];
  int read_status = read(fd, story, 8000000);
  if (read_status == -1) {
    printf("Can't read story: %s\n", strerror(errno));
    return 1;
  }


  printf("length:%d, %ld",length,lseek(fd,length,SEEK_END));
  char * last_input = story + lseek(fd,length,SEEK_END) -2;
  printf("LAST ENTRY\n====================\n%s\n",last_input);

  printf("Enter the next sentence in the story:\n");
  char input[100000];
  fgets(input, 100000, stdin);
  input[strlen(input)] = '\n';
  int new_line_length = strlen(input);

  fd = open("story.txt", O_WRONLY | O_APPEND);
  if (fd == -1) {
    printf("Can't open story: %s\n", strerror(errno));
  }

  int wr = write(fd, input, new_line_length);
  if (wr == -1) {
    printf("%s\n", strerror(errno));
  }
  printf("Added new entry to story.\n");

  //changing the shared memory
  new_line_length -= 2;
  data = &new_line_length;

  //Ups it back
  buf.sem_op = 1;
  semop(sem_id, &buf, 1);

  return 0;
}
