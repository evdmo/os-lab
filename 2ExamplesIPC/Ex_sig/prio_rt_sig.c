#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

// проверяем наличие приоритетов сигналов РВ

// функция для проверки успешного завершения (можно не использовать)
int success(int value) {
	if (value == -1) {
		perror("Failed success check");
		exit(1);
	} else {
		return value;
	}
}

// Собственый обработчик сигналов
void handler(int sig, siginfo_t* info, void* ucontext) {
	printf("Received signal %d, si_value %d\n", sig, info->si_value.sival_int);
}

int main(int argc, char** argv) {
	struct sigaction act = { 0 };
	act.sa_sigaction = handler;
	act.sa_flags |= SA_SIGINFO;
	// Без этой строки порядок выполнения сигналов будет обратным
	sigfillset(&act.sa_mask); // блокировка др.сигналов на время работы обработчика текущего 

	pid_t pid = success(fork());
	if (pid != 0) { // Родительский процесс
		printf("Parent PID %d\n", getpid());
		sleep(1);
		
		// Отправляем все сигналы РВ от SIGRTMIN по SIGRTMAX дважды (или лучше в обратном порядке от SIGRTMAX к SIGRTMIN)
		for (int i = 0; i < 2; i++) {
			for (int sig = SIGRTMIN; sig <= SIGRTMAX; sig ++) {
				union sigval value;
				value.sival_int = i;
				
				sigqueue(pid, sig, value);
				printf("RT signal sent %d\n", sig);
			}
		}
		
		wait(NULL);
		printf("Parent finished successfully!\n");
	} else { // процесс - потомок
		printf("son PID %d\n", getpid());
		for (int sig = SIGRTMIN; sig <= SIGRTMAX; sig ++) { // изменяем действие при получении сигналов
			success(sigaction(sig, &act, NULL)) ;
		}
		
		sleep(2);
		printf("Son finished successfully!\n");
	}
	
	return 0;
}