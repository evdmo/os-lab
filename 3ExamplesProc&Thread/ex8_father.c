#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

// применяем разные функции семейства exec (передаем переменные окружения и аргументы)

const char* SON_PATH = "son.out";
const char* SON_FILE = "./son.out";
const char* SON_ENV[] = { "HOME=SPb", (char*)NULL };

void execute(int n) {
	pid_t pid = fork();
	if (pid == -1) {
		perror("Failed to fork");
		exit(1);
	}
	
	if (pid == 0) { // gjnjvjr
		char buffer[80];
		sprintf(buffer, "%d", n);
		
		char* ARGS[] = { SON_PATH, buffer, (char*)NULL };
		
		switch (n) {
			case 0: execl(SON_PATH, SON_PATH, buffer, (char*)NULL); break;
			case 1: execlp(SON_FILE, SON_PATH, buffer, (char*)NULL); break;
			case 2: execle(SON_PATH, SON_PATH, buffer, (char*)NULL, SON_ENV); break;
			case 3: execv(SON_PATH, ARGS); break;
			case 4: execvp(SON_FILE, ARGS); break;
		default: printf("Incorrect n\n"); return;
		}
		
		// Исполнение придет в эту точку кода, только если вызов exec завершится ошибкой
		perror("Failed to exec");
		exit(1);
	}
}

int main() {
    for (int n = 0; n < 5; n++) {
		execute(n);
		
		// ждем завершения потомка, прежде чем продолжить
		int status;
		wait(&status);
		
		printf("Son %d finished with status %d\n", n, status);
	}

    return 0;
}

