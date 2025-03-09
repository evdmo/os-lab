#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_t t1,t2;

void SIGUSR1_handler(int sig){
    printf("SIGUSR1 получен второй нитью\n");
    pthread_exit(NULL);
}

void* ThreadFunc1(void* arg){
    int cnt = 1;
    printf("Первая нить запустилась\n");
    for(int i = 0; i < 5; ++i, ++cnt){
        sleep(5);
        printf("Первая нить: %d\n", cnt);
        pthread_kill(t2, SIGUSR1); 
    }
    pthread_exit(NULL);
}


void* ThreadFunc2(void* arg){
    int cnt = 1;
    printf("Вторая нить запустилась\n");
    system("ps -T");
    signal(SIGUSR1, SIGUSR1_handler);
    for(int i = 0; i < 10; ++i, ++cnt){
        sleep(1);
        printf("Вторая нить: %d\n", cnt);
    }
    pthread_exit(NULL);
 }

int main(){
    pthread_create(&t1, NULL, ThreadFunc1, NULL);
    pthread_create(&t2, NULL, ThreadFunc2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0; 
}
