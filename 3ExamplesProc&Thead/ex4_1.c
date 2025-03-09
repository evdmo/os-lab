#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

// пытаемся поменять параметры планирования (требует sudo) и наблюдать конкуренцию за процессор в новых условиях

const int STOP = 12;

long cnt(int to) {
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

    printf("Start with pid %d\n", pid);
    if(pid != 0) { // родительский процесс
        for (int i = 0; i < STOP; i++) {
            printf("parent iteration %d pid %d, counted to %ld\n", i, pid, cnt(i));
        }
        
        sleep(1); 
    } else { // потомок
	   struct sched_param scheduler_params;
	   scheduler_params.sched_priority = sched_get_priority_max(SCHED_FIFO);
	   if (sched_setscheduler(0, SCHED_FIFO, &scheduler_params) == -1) {
		perror("Failed to set scheduler");
		exit(1);
	   }

        for (int i = 0; i < STOP; i++) {
            printf("son iteration %d of son with pid %d, counted to %ld\n", i, pid, cnt(i));
        }
    }
	
    printf("Finished program with pid %d\n", pid);
    return 0;
}

