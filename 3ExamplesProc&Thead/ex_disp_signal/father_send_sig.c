#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2, pid3; // PID процессов-сыновей

    if((pid1 = fork()) == 0) {           // Последовательное формирование процессов-сыновей
        execl("son1.out", "son1.out", NULL);
    } if((pid2 = fork()) == 0) {
        execl("son2.out", "son2.out", NULL);
    } if((pid3 = fork()) == 0) {
        execl("son3.out", "son3.out", NULL);
    }

    system("echo Active processes: > result.txt && ps -l >> result.txt");

	// Посыл пользовательского сигнала SIGUSR1 трем сыновьям
    kill(pid1, SIGUSR1);
    kill(pid2, SIGUSR1);
    kill(pid3, SIGUSR1);
    
    system("echo >> result.txt && echo Proceses after sending signal: >> result.txt && ps -l >> result.txt");
}
