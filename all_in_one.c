#include <assert.h>
#include <bits/pthreadtypes.h>
#include <ctype.h>
#include <fcntl.h>
#include <linux/falloc.h>
#include <mqueue.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void *thread_fnct(void *args) { return args; }

int main(int argc, char **argv) {

  // semaphore // -pthread
  sem_t semaphore;
  sem_init(&semaphore, true, 10);
  sem_destroy(&semaphore);

  // mutex // -pthread
  pthread_mutex_t mutex;
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_lock(&mutex);
  pthread_mutex_unlock(&mutex);
  pthread_mutex_destroy(&mutex);

  // message queue
  char *name = argv[1];
  const int oflag = O_WRONLY;
  mqd_t mq_des = mq_open(name, oflag);
  mq_send(mq_des, name /*char* to msg*/, strlen(name), 1 /*priority*/);
  mq_receive(mq_des, name, strlen(name), NULL);
  mq_close(mq_des);
  
  // fork
  pid_t process_id = fork();
  if (process_id == 0) {
    exit(EXIT_SUCCESS);
  }
  waitpid(process_id, NULL, 0);

  // threads // -pthread
  pthread_t thread_id;
  pthread_create(&thread_id, NULL, thread_fnct, NULL);
  pthread_join(thread_id, NULL);

  // shared memory // -lrt
  const char *name2 = "/shared memory";
  const int oflag2 = O_CREAT | O_EXCL | O_RDWR;
  const size_t shared_mem_size = sizeof(int);
  const int fd_shm = shm_open(name2, oflag2, 0);
  ftruncate(fd_shm, shared_mem_size);
  int *shared_int =
      mmap(NULL, shared_mem_size, PROT_WRITE, MAP_SHARED, fd_shm, 0);
  munmap(shared_int, shared_mem_size);
  close(fd_shm);
  shm_unlink(name2);

  // pipes
  int pipe_des[2];
  pipe(pipe_des);
  // dup2(fd_1[1], STDOUT_FILENO); instead of stdout it will go to fd_1[1]!!
  // execl("/bin/grep", "/bin/grep", "-v", "lab", NULL);
  close(pipe_des[0]);
  close(pipe_des[1]);

  // environment variables
  const char *value_of_env = getenv("NAME");

  // condition variable
  pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
  // pthread_cond_wait(&cond_var, &mutex);
  pthread_cond_signal(&cond_var);
  pthread_cond_destroy(&cond_var);

  // barriers
  pthread_barrier_t barrier;
  pthread_barrier_init(&barrier, NULL, 1);
  pthread_barrier_wait(&barrier);
  pthread_barrier_destroy(&barrier);

  // socket
  // server
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  const int port = 8000;
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  // server -->
  bind(server_socket, (__SOCKADDR_ARG)&server_addr, sizeof(struct sockaddr_in));
  listen(server_socket, 1);
  int connection_socket;
  struct sockaddr_in connection_addr;
  socklen_t addr_len = sizeof(connection_addr);
  connection_socket =
      accept(server_socket, (__SOCKADDR_ARG)&connection_addr, &addr_len);
  // client -->
  // connect(client_socket, const struct sockaddr *addr, socklen_t len)
  // also important
  // write(); read(); bzero();

  // own plugins exc11

  return EXIT_SUCCESS;
}