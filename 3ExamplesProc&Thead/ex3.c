#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// аналог примера ex2, продолжительные циклы оформим продолжительной функцией 
// пытаемся наблюдать конкуренцию процессов за процессор при параметрах планирования, установленных по умолчанию  
const int STOP = 12;

long cnt(int to) { // Продолжительная функция
    long r = 0;
    for (int j = 0; j < to * 1000000; j++){
        r += j;
    }
    
    return r;
}

int main() {
    pid_t pid = fork();
    if(pid == -1) {
        perror("Failed to fork");
        exit(1);
    }

    printf("Program started with pid %d\n", pid);
    if(pid != 0) { // Родительский процесс
        for (int i = 0; i < STOP; i++) { // на каждой итерации выводим информацию о процессе
            printf("parent iteration %d  pid %d, counted to %ld\n", i, pid, cnt(i));
        }
        
        sleep(1); 
    } else { // потомок
	pid_t pid = getpid();
        for (int i = 0; i < STOP; i++) { 
            printf("son iteration %d pid %d, counted to %ld\n", i, pid, cnt(i));
        }
    }
	
    printf("Finished program with pid %d\n", pid);
    return 0;
}

