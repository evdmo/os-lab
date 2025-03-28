#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>


// пытаемся поменять параметры планирования и наблюдать конкуренцию за процессор в новых условиях

const int STOP = 12;
long cnt(int to) {
    long r = 0;
    for (int j = 0; j < to * 10000000; j++){
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
	   
	struct sched_param scheduler_params;
	scheduler_params.sched_priority = 20;
	if (sched_setscheduler(pid, SCHED_FIFO, &scheduler_params) == -1) {
		perror("Failed to set scheduler");
		exit(1);
	}

    printf("Started program with pid %d\n", pid);
    if(pid != 0) { // родительский процесс
        for (int i = 0; i < STOP; i++) {
            printf("parent iteration %d o pid %d, counted to %ld\n", i, pid, cnt(i));
        }
        
        sleep(1); 
    } else { // потомок
        for (int i = 0; i < STOP; i++) {
            printf("son iteration %d o, counted to %ld\n", i, cnt(i));
        }
    }
	
    printf("Finished program with pid %d\n", pid);
    return 0;
}

