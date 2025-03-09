#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
 
void handle_signal(int sig) {
    printf("Получен сигнал %d\n", sig);
    exit(EXIT_SUCCESS); // Завершение программы после получения сигнала
}
 
int main() {
    printf("Мой PID = %d\n", getpid());
    
    signal(SIGRTMIN, handle_signal);
    
    while(1) {
        printf("Жду сигнала...\n");
        sleep(1);
    }
    
    return 0;
}
