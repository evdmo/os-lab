#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>

// отслеживаем потомка с заданным pid (варианты применения функций wait() и waitpid() )
// в "ex9_son" целесообразно рассмотреть разные варианты завершения потомка (return 0; exit(1); exit(-1); return var;) 

int main() {
    printf("Father: pid %d ppid %d\n", getpid(), getppid());
	
	pid_t son_pid[4];
	for (int i = 0; i < 4; i++) {
		son_pid[i] = fork();
		if (son_pid[i] == -1) {
			perror("Failed to fork");
			exit(1);
		}
		
		if (son_pid[i] == 0) { // потомок
			char buffer[8];
			sprintf(buffer, "%d", i);
			
			execl("son.out", "son.out", buffer, NULL); // замещаем на нужный процесс
		}
	}

    for (int i = 0; i < 4; i++) {
		int result, status;
		siginfo_t infop;
		switch (i) {
			case 0: result = wait(&status); break; // ждем завершения любого потомка
			case 1: result = waitpid(-1, &status, 0); break; // аналогично case 0, но с использованием другой функции
			case 2: result = waitpid(son_pid[i], &status, 0); break; // ждем завершения конкретного потомка (с заданным pid)
			// ждем завершения конкретного потомка
			case 3: result = waitid(P_PID, son_pid[i], &infop, WEXITED); status = infop.si_status; break; 

		default: break;
		}
		
        printf("Son %d with pid %d was awaited with status %d, exited code: %d\n", i, result, status, (int)WEXITSTATUS(status));
    }

    return 0;
}

