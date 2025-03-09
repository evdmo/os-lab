#include "stdio.h"
#include "unistd.h"
#include "pthread.h"

// Создание нитей/потоков
const int STOPC= 10;
const int THREADS_COUNT = 5;
 
void* thread(void* args) {
  int n = *(int*)args;
  
  printf("Started thread, delay %d\n", n);
  
  for (int i = 0; i < STOPC; i++) {
    printf("Thread %d: counted to %d...\n", n, i);
    sleep(n);
  }
  
  printf("Finished thread, delay %d\n", n);
  
  pthread_exit(0);
}
 
int main() {
  pthread_t tid[THREADS_COUNT];
  pthread_attr_t tattr[THREADS_COUNT];
  int data[THREADS_COUNT];
  
  for (int n = 0; n < THREADS_COUNT; n++) {
    data[n] = n + 1;
    
    pthread_attr_init(&tattr[n]);
    int r = pthread_create(&tid[n], &tattr[n], thread, (void*)&data[n]);
  }
  
  for (int n = 0; n < THREADS_COUNT; n++) {
    pthread_join(tid[n], NULL);
  }
  
  return 0;
}
