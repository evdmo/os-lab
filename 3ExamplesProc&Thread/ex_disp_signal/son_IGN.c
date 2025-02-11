#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(){
    signal(SIGUSR1, SIG_IGN);   // Действие при получение сигнала SIGUSR1: игнорировать данный сигнал
    sleep(5);   // Задержка, чтобы процесс не закончился быстрее отца
    return 0;
}
