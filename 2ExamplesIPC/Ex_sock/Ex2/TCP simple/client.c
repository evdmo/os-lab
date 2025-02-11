#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// client устанавливает соединение с сервером и отправляет введённое пользователем сообщение

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main(){
    int sock;
    struct sockaddr_in serv_addr;

    // Создание сокета
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("socket");
        exit(1);
    }

    // Данное поле всегда равно AF_INET
    serv_addr.sin_family = AF_INET;
    // Присваиваем номер порта, который намерен занять процесс
    serv_addr.sin_port = htons(PORT);

    // Преобразование структуры с сетевым адресом
    int res = inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);
    if(res <= 0){
        perror("inet_pton");
        exit(1);
    }

    // Установка связи с сервером
    res = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(res < 0){
        perror("connect");
        exit(1);
    }

    // Создание переменной для хранения сообщения и заполнение её нулями
    char message[100];
    bzero(message, 100);
    printf("Введите сообщение для передачи локально:\n");
    // Ввод сообщения бользователем
    fgets(message, 100, stdin);
    message[strlen(message) - 1] = '\0';

    // Отправка серверу сообщения
    send(sock, message, strlen(message), 0);
    printf("Сообщение (%s) отправлено серверу\n", message);

    close(sock);

    return 0;
}