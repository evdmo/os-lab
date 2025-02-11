// подробное описание см. в Уч.пос.S17-72 Душутина Е.В."СПО Межпроц.взаимод.в ОС"стр.27-31

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

// Определяем структуру, являющуюся сообщением
typedef struct{
    long msgtype;
    char msg[100];
}Message;

// Создаём очередь
int queue;

// Функция обработчика сигнала
void handler(int sig){
    // Восстанавливаем обработчик сигнала по умолчанию
    signal(sig, SIG_DFL);
    // Очищаем очередь
    if(msgctl(queue, IPC_RMID, 0) < 0){
        perror("delete_queue");
        exit(1);
    }
}

int main(){
    char key_file[10] = "key";

    // Создаём ключ для работы с очередью
    key_t key;
    key = ftok(key_file, 'Q');
    if(key == -1){
        perror("no_key_for_the_key_file");
        exit(1);
    }

    printf("Получатель сообщений\n");

    // Создаём очередь для сообщений
    queue = msgget(key, IPC_CREAT | 0666);
    if(queue < 0){
        perror("queue_create");
        exit(4);
    }

    // Замена обработчика сигнала, чтобы при завершении удалялась очередь
    signal(SIGINT, handler);

    printf("Очередь (id = %d) создана\n", queue);

    // Цикл работы сервера
    Message message;
    int res;

    while(1){
        // Очищаем сообщение
        bzero(message.msg, 100);
        printf("Ожидание сообщения\n");
        // Получаем первое сообщение (тип 1)
        res = msgrcv(queue, &message, sizeof(Message), 1L, 0);
        if(res < 0){
            perror("recieve_message");
            // Завершаем процесс
            kill(getpid(), SIGINT);
        }
        printf("Ответ клиента: %s\n", message.msg);
        // Посылаем клиенту сообщение (тип 2), что всё успешно
        message.msgtype = 2L;
        // Очищаем сообщение и записываем новое
        bzero(message.msg, 100);
        strcpy(message.msg, "OK");
        // Отправляем сообщение
        res = msgsnd(queue, (void*)&message, sizeof(Message), 0);
        if(res != 0){
            printf("send_message");
            kill(getpid(), SIGINT);
        }
        printf("Отправлено сообщение клиенту: %s\n", message.msg);
    }

    return 0;
}