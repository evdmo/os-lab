#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int common_variable = -1;
const int STOP_AT = 12;

int main() {
    pid_t pid = fork();
    if(pid == -1) {
        perror("Failed to fork");
        exit(1);
    }

    printf("Program started with pid %d\n", pid);
    if(pid != 0) { // Родительский процесс
	common_variable = 0;
        for (int i = 0; i < STOP_AT; i++) {
	    int r = 0;
            for (int j = 0; j < 100000; j++) r += j; // Заходим в продолжительный цикл, чтобы продлить время работы процесса

            printf("Iteration %d of parent with pid %d, r = %d\n", i, pid, r);

        }
        
        sleep(1); // Даём возможность дочернему процессу завершиться
    } else { // Дочерний процесс
	common_variable = 1;
        for (int i = 0; i < STOP_AT; i++) {
	    int r = 0;
            for (int j = 0; j < 100000; j++) r += j; // Заходим в продолжительный цикл, чтобы продлить время работы процесса

            printf("Iteration %d of son with pid %d, r = %d\n", i, pid, r);
        }
    }
	
    printf("Common variable is %d\nFinished program with pid %d\n", common_variable, pid);
    return 0;
}

