// подробное описание см. в Уч.пос.S17-72 Душутина Е.В."СПО Межпроц.взаимод.в ОС"стр.40-44
// полный квант описания примеров по этой теме со стр.32.

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "shm.h"
#include <string.h>

int* buf;

int main(int argc, char** argv) {
    char keyFile[100];
    bzero(keyFile,100);
    if(argc < 2) {
        printf("Используется ключевой файл по умолчанию %s\n",DEF_KEY_FILE);
        strcpy(keyFile,DEF_KEY_FILE);
    } else
        strcpy(keyFile,argv[1]);

    key_t key;
    int shmemory;
    int semaphore;

    // Будем использовать один и тот же ключ для семафора и разделяемой памяти
    if((key = ftok(keyFile, 'Q')) < 0) {
        printf("Не удалось получить ключ для ключевого файла %s и id 'Q'\n",keyFile);
        exit(1);
    }

    // Создание разделяемой памяти
    if((shmemory = shmget(key, (BUF_SIZE+1)*sizeof(int), 0666)) < 0) {
        printf("Не удается создать разделяемую память\n");
        exit(1);
    }

    // Присоединение разделяемой памяти к адресному пространству процесса
    if((buf = (int*)shmat(shmemory, 0, 0)) < 0) {
        printf("Ошибка при присоединении разделяемой памяти\n");
        exit(1);
    }

    // Создание семафора
    if((semaphore = semget(key, 2, 0666)) < 0) {
        printf("Ошибка при создании семафора\n");
        exit(1);
    }

    printf("Нажмите Enter, чтобы начать работу\n");
    getchar();

    int send = 0;
    char tb[10];
    int i = 0;
    for(i = 0; i < 10;++i) {
        // Ждем, пока будет хоть одна свободная ячейка
        if(semop(semaphore, waitNotFull, 1) < 0) {
            printf("Не удалось выполнить операцию\n");
            exit(1);
        }

        // Ждем доступа к разделяемой памяти
        if(semop(semaphore, mem_lock, 1) < 0) {
            printf("Не удалось выполнить операцию\n");
            exit(1);
        }

        printf("Добавляем %d в ячейку %d\n",send,buf[BUF_SIZE]+1);
        ++buf[BUF_SIZE];
        buf[buf[BUF_SIZE]] = send++;

        // Освобождаем доступ к памяти
        if(semop(semaphore, mem_unlock, 1) < 0) {
            printf("Не удалось выполнить операцию\n");
            exit(1);
        }

        // Увеличиваем число занятых ячеек
        if(semop(semaphore, releaseFull, 1) < 0) {
            printf("Не удалось выполнить операцию\n");
            exit(1);
        }
    }

    // Отключение от области разделяемой памяти
    shmdt(buf);

    return 0;
}

