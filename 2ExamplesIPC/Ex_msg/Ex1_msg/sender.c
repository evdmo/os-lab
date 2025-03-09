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

int main(){
    char key_file[10] = "key";

    key_t key;
    key = ftok(key_file, 'Q');
    if(key == -1){
        perror("no_key_for_the_key_file");
        exit(1);
    }

    // Создаём сообщения
    queue = msgget(key, 0);
    if(queue < 0){
        perror("queue_create");
        exit(4);
    }

    printf("Отправитель сообщения (id очереди = %d)\n", queue);

    // Цикл работы клиента
    Message message;
    int res;
    while(1){
        // Очищаем содержимое сообщения
        bzero(message.msg, 100);
        printf("Введите сообщение:\n");
        // Считываем строку из терминала
        fgets(message.msg, 100, stdin);
        // Устанавливаем символ конца строки
        message.msg[strlen(message.msg) - 1] = '\0';
        // Определяем тип сообщения
        message.msgtype = 1L;
        // Отправляем сообщение
        res = msgsnd(queue, (void*)&message, sizeof(Message), 0);
        if(res != 0){
            perror("send_message");
            exit(1);
        }
        printf("Сообщение (%s) отправлено\n", message.msg);
        // Получаем сообщение (тип 2) от сервера
        res = msgrcv(queue, &message, sizeof(Message), 2L, 0);
        if(res < 0){
            perror("recieve_message");
            exit(1);
        }
        printf("Ответ сервера: %s\n", message.msg);
    }

    return 0;
}