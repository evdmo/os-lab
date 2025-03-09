// подробное описание см. в Уч.пос.S17-72 Душутина Е.В."СПО Межпроц.взаимод.в ОС"стр.40-44
// полный квант описания примеров по этой теме со стр.32.

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include "shm.h"
#include <string.h>

int* buf;
int shmemory;
int semaphore;

// Обработчик сигнала SIGINT
void intHandler(int sig) {
    shmdt(buf); // Отсоединяемся от разделяемой памяти
    shmctl(shmemory, IPC_RMID, 0); // Удаляем разделяемую память
    semctl(semaphore, 0, IPC_RMID); // Удаляем семафор
}

int main(int argc, char** argv) {
    char keyFile[100];
    bzero(keyFile,100);
    if(argc < 2) {
        printf("Используется ключевой файл по умолчанию %s\n",DEF_KEY_FILE);
        strcpy(keyFile,DEF_KEY_FILE);
    } else
        strcpy(keyFile,argv[1]);

    key_t key;
    if((key = ftok(keyFile, 'Q')) < 0) {
        printf("Не удалось получить ключ для ключевого файла %s и id 'Q'\n",keyFile);
        exit(1);
    }

    // Создание разделяемой памяти
    if((shmemory = shmget(key,(BUF_SIZE+1)*sizeof(int),IPC_CREAT|0666))< 0)  {
        printf("Не удается создать разделяемую память\n");
        exit(1);
    }

    // Присоединение разделяемой памяти к адресному пространству процесса
    if((buf = (int*)shmat(shmemory, 0, 0)) < 0) {
        printf("Ошибка при присоединении разделяемой памяти\n");
        exit(1);
    }

    // Установка обработчика сигнала SIGINT
    signal(SIGINT, intHandler);

    // Создание группы из трех семафоров
    if((semaphore = semget(key, 3, IPC_CREAT | 0666)) < 0) {
        printf("Ошибка при создании семафора\n");
        kill(getpid(),SIGINT);
    }

    // Установка индекса в -1 для контроля доступа к разделяемой памяти
    buf[BUF_SIZE] = -1; // инициализация массива -1
    int j = 0;
    for(j = 0; j < BUF_SIZE; ++j) {
        buf[j] = -1;
    }
    // Установка семафора в число свободных ячеек, что позволяет писать
    if(semop(semaphore, setFree, 1) < 0) {
        printf("Выполнение завершено\n");
        kill(getpid(),SIGINT);
    }

    // Отключение блокировки разделяемой памяти
    if(semop(semaphore, mem_unlock, 1) < 0) {
        printf("Выполнение завершено\n");
        kill(getpid(),SIGINT);
    }

    printf("Нажмите Enter, чтобы начать работу\n");
    getchar();

    // Основной цикл работы
    int i = 0;
    for(i = 0; i < 15; ++i) {
        // Ожидание, пока будет хоть одна непустая ячейка
        if(semop(semaphore, waitNotEmpty, 1) < 0) {
            printf("Выполнение завершено\n");
            kill(getpid(),SIGINT);
        }

        // Ожидание возможности поработать с разделяемой памятью
        if(semop(semaphore, mem_lock, 1) < 0) {
            printf("Выполнение завершено\n");
            kill(getpid(),SIGINT);
        }

        // Чтение сообщения от клиента
        int res = buf[buf[BUF_SIZE]];
        buf[BUF_SIZE] = buf[BUF_SIZE] - 1;
        printf("Удалить %d из ячейки %d\n", res,buf[BUF_SIZE]+1);

        // Освобождение разделяемой памяти
        if(semop(semaphore, mem_unlock, 1) < 0) {
            printf("Выполнение завершено\n");
            kill(getpid(),SIGINT);
        }

        // Увеличение числа пустых ячеек
        if(semop(semaphore, releaseEmpty, 1) < 0) {
            printf("Выполнение завершено\n");
            kill(getpid(),SIGINT);
        }
    }
}

