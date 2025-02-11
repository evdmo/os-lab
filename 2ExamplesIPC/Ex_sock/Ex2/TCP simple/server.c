#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// server ожидает соединения с клиентом, после чего принимает сообщение от него и выводит в терминал

// Определяем порт для передачи
#define PORT 8080

int main(){
    // Создаём переменные для сокетов
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;

    // Создание сокета (доставка гарантируется в порядке поступления)
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == 0){
        perror("server_socket_create");
        exit(1);
    }

    // Данное поле всегда равно AF_INET
    server_address.sin_family = AF_INET;
    // Указываем все адреса локального хоста
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    // Присваиваем номер порта, который намерен занять процесс
    server_address.sin_port = htons(PORT);

    // Привязка сокета
    int res = bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if(res < 0){
        perror("bind");
        exit(1);
    }

    // ИНформируем ОС об ожидании запросов связи на данном сокете
    res = listen(server_socket, 5);
    if(res < 0){
        perror("listen");
        exit(1);
    }

    printf("Ожидание соединения\n");

    // Принимаем соединение
    int address_len = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&address_len);
    if(client_socket < 0){
        perror("accept");
        exit(1);
    }

    printf("Ожидание сообщения\n");

    // Получаем от клиента сообщение
    char buf[100];
    while(1){
        // Очищаем буфер
        bzero(buf, 100);
        // Получаем сообщение
        res = read(client_socket, buf, 100);
        if(res <= 0){
            break;
        }
        printf("Получено сообщение от клиента: %s\n", buf);
    }

    return 0;
}