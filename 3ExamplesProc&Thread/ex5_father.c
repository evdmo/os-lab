#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>

// "отселяем" потомка в отдельный файл
int main(){
    int status;
    pid_t pid = getpid();
    pid_t ppid = getppid();
    printf("Parent: pid=%d, ppid=%d\n", pid, ppid);

    if(fork() == 0){
        execl("son.out", "son.out", NULL);
    }

    system("ps -xf > proc_tab1.txt"); // получаем таблицу процессов до завершения потомка, используя утилиту ps
    wait(&status);
    system("ps -xf > proc_tab2.txt"); // таблица после завершения потомка
    printf("Son finished with status %d\n", status);
}

