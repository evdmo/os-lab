#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(){
    signal(SIGUSR1, SIG_DFL);   // Действие при получение сигнала SIGUSR1: действовать по-умолчанию (обусловлено SIG_DFL)
    sleep(5);   // Задержка, чтобы процесс не закончился быстрее отца
    return 0;
}
