#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
 
int main() {
    pid_t target_pid;
    printf("Введите PID выбранного процесса: "); //целесообразно задействовать 2 терминала для работы программ отправителя и получателя
    scanf("%d", &target_pid);
    
    printf("Отправка сигнала процессу с PID %d\n", target_pid);
    kill(target_pid, SIGRTMIN);
    
    return 0;
}
