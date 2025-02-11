#include "stdlib.h"
#include "stdio.h"
#include "signal.h"
#include "unistd.h"
#include "pthread.h"
#include "sys/syscall.h"
 
const int STOPC = 10;
const int THREADS_COUNT = 5;
 
void* thread(void* args) {
  int n = *(int*)args;
  char ps_cmd1[128];
  char ps_cmd2[128];
  
  sprintf(ps_cmd1, "ps -axhf >thread%d.txt", n);
  sprintf(ps_cmd2, "ps -axhf >>thread%d.txt", n);
  
  system(ps_cmd1);
  
  int pid = getpid();
  int tid = syscall(SYS_gettid);
  
  printf("Started thread, delay %d; process id: %d, thread id: %d\n", n, pid, tid);
  
  for (int i = 0; i < STOPC; i++) {
    printf("Thread %d: counted to %d...\n", n, i);
    sleep(n);
  }
  
  printf("Finished thread, delay %d\n", n);
  
  system(ps_cmd2);
  
  // Trying to kill the thread
  kill(tid, SIGKILL);
  
  pthread_exit(0);
}
 
int main() {
  pthread_t tid[THREADS_COUNT];
  pthread_attr_t tattr[THREADS_COUNT];
  int data[THREADS_COUNT];
  
  system("ps -axhf >file.txt");
  
  for (int n = 0; n < THREADS_COUNT; n++) {
    data[n] = n + 1;
    
    pthread_attr_init(&tattr[n]);
    int r = pthread_create(&tid[n], &tattr[n], thread, (void*)&data[n]);
  }
  
  system("ps -axhf >>file.txt");
  
  for (int n = 0; n < THREADS_COUNT; n++) {
    pthread_join(tid[n], NULL);
  }
  
  system("ps -axhf >>file.txt");
  
  return 0;
}
