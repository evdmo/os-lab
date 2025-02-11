#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();
    if(pid == -1) { // fork() возвращает -1 при неудаче
        perror("Failed to fork");
        exit(1);
    }

    printf("Program started with pid %d\n", pid);
    if(pid != 0) { // Родительский процесс
        int parent_value = 0;
        for (int i = 0; i < 128; i++) {
            parent_value += i;
        }
        
        printf("Parent variable: %d\n", parent_value);
        sleep(1); // (не обязательно)
    } else { // потомок, pid == 0
        int son_value = 0;
        for (int i = 0; i < 128; i++) {
            son_value += i * i;
        }
        
        printf("Son variable: %d\n", son_value);
    }

    printf("Finished program with pid %d\n", pid);
    return 0;
}

