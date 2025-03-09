#include "stdio.h"
#include "unistd.h"
#include "pthread.h"
#include "stdlib.h"
#include "sched.h"
#include "time.h"

// простой пример изменения политики планирования для нитей
//создаем 10 нитей и запускаем их, задавая всем одну политику планирования и, если возможно, приоритет; затем меняем политику и ее параметры
// требует sudo
 
const int STOPС = 1000;
const int THREADS_COUNT = 10;
 
void million(){
  int ind = 0;
  while (ind != 1000000){
    ind += 1;
  }
}
 
void* thread(void* args) {
  int n = *(int*)args;
  
  int time_used = 0;
  clock_t temp_time_start = 0;
  clock_t temp_time_stop = 0;
  
  int policy;
  struct sched_param param;
  pthread_getschedparam(pthread_self(), &policy, &param);
  printf("старт нити %d; с приоритетом %d\n", n, param.sched_priority);
  
  temp_time_start = clock();
  for (int i = 0; i < STOPС; i++) {
    million();
  }
  temp_time_stop = clock();
  time_used = temp_time_stop - temp_time_start;
  
  printf("Завершение нити %d; время %d\n", n, time_used);
  
  pthread_exit(0);
}
 
int main() {
  pthread_t tid[THREADS_COUNT];
  pthread_attr_t tattr[THREADS_COUNT];
  int data[THREADS_COUNT];
  
  int policy;
  struct sched_param param;
  sched_getparam(0, &param);
  param.sched_priority = 50;
  sched_setscheduler(0, SCHED_FIFO, &param);
  
  printf ("Текущая политика планирования процесса: ");
        switch (sched_getscheduler (0)) {
		case SCHED_FIFO:
			printf ("SCHED_FIFO\n");
			break;
		case SCHED_RR:
			printf ("SCHED_RR\n");
			break;
		case SCHED_OTHER:
			printf ("SCHED_OTHER\n");
			break;
		case -1:
			perror ("SCHED_GETSCHEDULER");
			break;
		default:
			printf ("Неизвестная политика планирования\n");
		}
  printf("Приоритет процесса %d\n", param.sched_priority);
  
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
