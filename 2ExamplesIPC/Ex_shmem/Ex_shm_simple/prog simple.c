// Программа для демонстрации применения базовых функций работы с памятью и семафорами
// Потомок генерирует случ. числа и записывает их в разделяемую память 
// Родитель считывает из памяти введённую информацию и выводит её на экран
// Для контроля доступа к памяти используется семафор

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

#define KEY 42
#define NUM 5

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// Функция, в которой происходит инициализация семафора
int sem_init(int sem_id, int sem_num, int value){
    union semun arg;
    arg.val = value;
    // Устанавливаем значение семафора
    return semctl(sem_id, sem_num, SETVAL, arg);
}

// Функция ожидания семафора
void sem_wait(int sem_id, int sem_num){
    struct sembuf sembuf;
    sembuf.sem_num = sem_num;
    sembuf.sem_flg = 0;
    sembuf.sem_op = -1;
    semop(sem_id, &sembuf, 1);
}

// Функция освобождения семаформа
void sem_free(int sem_id, int sem_num){
    struct sembuf sembuf;
    sembuf.sem_num = sem_num;
    sembuf.sem_flg = 0;
    sembuf.sem_op = 1;
    semop(sem_id, &sembuf, 1);
}

int main(){
    // Создание сегмента разделяемой памяти
    int shm_id = shmget(KEY, 1024, IPC_CREAT | 0666);
    if(shm_id < 0){
        perror("shmget");
        exit(1);
    }
    // Подключение сегмента к адресному пространству процесса
    int *shm_addr = (int *)shmat(shm_id, NULL, 0);
    if(shm_addr == (int *)-1){
        perror("shmat");
        exit(1);
    }
    // Создание семафора
    int sem_id = semget(KEY, 1, IPC_CREAT | 0666);
    if(sem_id < 0){
        perror("semget");
        exit(1);
    }

    // Инициализация семаформа
    sem_init(sem_id, 0, 1);

    printf("Создание дочернего процесса\n");
    // Создаём дочерний процесс
    int pid = fork();
    if(pid == 0){
        int value;
        printf("> Генерация рандомных чисел дочерним процессом\n");

        // Инициализация генератора случайных числе
        srand(time(NULL));
        // Запись чисел в разделяемую память
        for(int i = 0; i < NUM; i++){
            // Ожидание доступа к разделяемой памяти
            sem_wait(sem_id, 0);

            value = rand() % 1000;
            shm_addr[i] = value;

            // Освобождение семафора
            sem_free(sem_id, 0);

            printf("> (%d) В разделяемую память было записано число %d\n", i + 1, shm_addr[i]);
        }
    }
    else{
        // Ожидаем немного, пока дочерний процесс выполняет действия
        sleep(5);

        // Ожидание доступа к разделяемой памяти
        sem_wait(sem_id, 0);

        printf("Родительский процесс считывает из разделяемой памяти числа\n");
        for(int i = 0; i < NUM; i++){
            printf("Считано число %d\n", shm_addr[i]);
        }

        // Освобождение семафора
        sem_free(shm_id, 0);

        // Дожидаемся завершения дочернего процесса
        wait(NULL);
    }

    // Удаление семафора
    semctl(sem_id, 0, IPC_RMID);
    shmdt(shm_addr);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}