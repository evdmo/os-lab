#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Определяем порт для передачи
#define PORT 8080

int main(){
    // Создаём переменные для сокетов
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;

    // Создание сокета (посылка отдельных пакетов информации)
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
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

    printf("Ожидание сообщений\n");

    int address_len = sizeof(client_address);
    // Получаем от клиента сообщения
    char buf[100];
    while(1){
        // Очищаем буфер
        bzero(buf, 100);
        // Получаем сообщение
        res = recvfrom(server_socket, buf, 100, 0, (struct sockaddr *)&client_address, (socklen_t *)&address_len);
        if(res < 0){
            perror("recvfrom");
            exit(1);
        }
        printf("Получено сообщение от клиента: %s\n", buf);
    }

    return 0;
}