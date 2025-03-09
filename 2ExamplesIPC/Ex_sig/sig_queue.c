#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

// Создаем очередь из обычных сигналов и сигналов реального времени (РВ), чтобы исследовать порядок их исполнения

// функция для проверки успешного завершения используемых системных функций (для сокращения объема кода, можно не использовать)
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
	static int s_counter = 0; // Чтобы различать одинаковые сигналы
	
	int counter = ++s_counter;
	printf("Received signal %d (no %d), si_code %d, si_value %d\n", sig, counter, info->si_code, info->si_value.sival_int);
	
	// ... сюда целесообразно включить длительно исполняемый вычислит.цикл (чтобы успеть с обработкой и выводом информации)

	printf("Finished handling signal %d (no %d)\n", sig, counter);
}

int main(int argc, char** argv) {
	struct sigaction act = { 0 };
	act.sa_sigaction = handler;
	act.sa_flags |= SA_SIGINFO;  // важный флаг для гарантии обработки сигнала с номером РВ именно как сигнала РВ
	
	// важно блокировать все сигналы на время работы обработчика (установкой всех сигналов в act.sa_mask)
	sigfillset(&act.sa_mask); // без этой строки порядок выполнения сигналов будет обратным!
	
	pid_t pid = success(fork());
	if (pid != 0) { // Родительский процесс
		printf("Parent PID %d\n", getpid());
		sleep(1);
		
		// здесь можно задавать разные комбинации сигналов для экспериментов
		int SIGNALS[] = { SIGUSR1, SIGRTMIN, SIGRTMAX, SIGUSR2, SIGUSR1, SIGUSR1, SIGRTMIN, SIGRTMAX, SIGUSR2 };
		for (int i = 0; i < 9; i++) {
			int sig = SIGNALS[i];
			if (sig >= SIGRTMIN) { // Отправка сигнала реального времени
				union sigval value;
				value.sival_int = i; //порядковый номер отправки сигнала, попробуем его передать с сигналом, как value
				
				sigqueue(pid, sig, value); //ф-ция для отправки сигнала РВ, позволяет передать доп.значение value
				printf("RT signal sent %d\n", sig);
			} else { // Отправка обычного сигнала
				kill(pid, sig);
				printf("Common signal sent %d\n", sig);
			}
		}
		
		wait(NULL);
		printf("Parent finished successfully!\n");
	} else { // процесс-потомок
		if (success(sigaction(SIGUSR1, &act, NULL)) == 0 
			&& success(sigaction(SIGUSR2, &act, NULL)) == 0 
			&& success(sigaction(SIGRTMIN, &act, NULL)) == 0 
			&& success(sigaction(SIGRTMAX, &act, NULL)) == 0) { // Пробуем изменить действие при получении сигналов
			
			printf("Son PID %d\n", getpid());
			sleep(2);
			printf("Son finished successfully!\n");
		}
	}
	
	return 0;
}