#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

int main(){
    pid_t pid = getpid();
    pid_t ppid = getppid();
    printf("Son: pid=%d, ppid=%d\n", pid, ppid);

    volatile long r = 0;
    for (int i = 0; i < 10000000; i++) r += i; // добавим задержку, чтобы работа процесса успела отобразиться в таблице

    return 0;
}

