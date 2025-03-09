#include <sched.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int pid = getpid();     // PID текущего процесса
    int ppid = getppid();   // PPID текушего процесса
	
	char n = argv[1][0];
	if (n == '1') {
		printf("Son 1: pid = %d ppid = %d (father created son and waits it to die)\n", pid, ppid);
		sleep(3);   // Ожидание, чтобы процесс-родитель не завершился, пока не завершится данный процесс-сын
	} else if (n == '2') {
		printf("Son 2: pid = %d ppid = %d (father terminated before son)\n", pid, ppid);  // Вывод информации о процессе
		sleep(10);  // Ожидание, чтобы процесс завершился после процесса-родителя
		
		ppid = getppid();
		printf("After sleep, son 2: pid = %d, ppid = %d\n", pid, ppid); // Повторный вывод информации
		system("ps xjf >> result.txt"); // Исполнение команды в терминале
	} else if (n == '3') {
		printf("Son 3: pid = %d, ppid = %d (son 3 is terminated before father is, it is zombie now)\n", pid, ppid);    // Вывод информации
	}
	
    return 0;
}
