#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

const uint64_t COUNT_TO = 100000000;

// Трудоёмкая функция
uint64_t mill(uint64_t multiplier) {
	volatile uint64_t result = 0;
	for (uint64_t i = 0; i < COUNT_TO * multiplier; i++) {
		result += i;
	}
	
	return result;
}

// Вспомогательная функция для проверки успешной отработки функций
int success(int value) {
	if (value == -1) {
		perror("Failed success check: ");
		exit(1);
	} else {
		return 1;
	}
}

// Собственый обработчик сигналов
void handler(int sig) {
	printf("Received signal %d\n", sig);
	printf("Handler of signal %d : counted to %lu; quitting handler\n", sig, mill(10));
}

int main(int argc, char** argv) {
	struct sigaction act = { 0 };
	act.sa_handler = handler;
	
	if (argc > 1 && argv[1][0] == '1') { // Если был передан аргумент 1, то игнорируем сигнал прерывания (Ctrl+C) во время работы обработчика
		sigaddset(&act.sa_mask, SIGINT);
	}
	
	if (success(sigaction(SIGUSR1, &act, NULL))) { // Пробуем изменить действие при получении SIGUSR1
		raise(SIGUSR1);
	}
	
	printf("Program finished successfully!\n");
	
	return 0;
}