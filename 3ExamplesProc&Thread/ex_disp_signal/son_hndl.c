#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void sig_hndl(){
    puts("Received signal SIGUSR1 in son 3");    // Вывод простого сообщения в терминал
}

int main(){
    signal(SIGUSR1, sig_hndl);     // Действие при получение сигнала SIGUSR1: вызвать функцию signalActions()
    sleep(5);   // Задержка, чтобы процесс не закончился быстрее отца
    return 0;
}
