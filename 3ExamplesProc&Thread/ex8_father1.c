#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

// применяем разные функции семейства exec (передаем переменные окружения и аргументы)
const char* SON_PATH = "echo";
const char* SON_FILE = "echo";
const char* SON_ENV[] = { (char*)NULL };

void execute(int n) {
	pid_t pid = fork();
	if (pid == -1) {
		perror("Failed to fork");
		exit(1);
	}
	
	if (pid == 0) { // потомок
		char* ARGS[] = { SON_PATH, "Hi", (char*)NULL };
		
		switch (n) {
			case 0: execl(SON_PATH, SON_PATH, "Hi", (char*)NULL); break;
			case 1: execlp(SON_FILE, SON_PATH, "Hi", (char*)NULL); break;
			case 2: execle(SON_PATH, SON_PATH, "Hi", (char*)NULL, SON_ENV); break;
			case 3: execv(SON_PATH, ARGS); break;
			case 4: execvp(SON_FILE, ARGS); break;
		default: return;
		}
		
		// придем сюда, только если вызов exec завершится ошибкой
		perror("Failed to exec");
		exit(1);
	}
}

int main() {
    for (int n = 0; n < 5; n++) {
		printf("Son %d: ", n);
		fflush(stdout);
		execute(n);
		
		// ждем завершения потомка, прежде чем продолжить
		int status;
		wait(&status);
		
		printf("Son %d finished with status %d\n", n, status);
	}

    return 0;
}

