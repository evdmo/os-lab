#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main(){
    int sock;
    struct sockaddr_in serv_addr;

    // Создание сокета (посылка отдельных пакетов информации)
    sock = socket(AF_INET, SOCK_DGRAM, 0);
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

    // Создание переменной для хранения сообщения и заполнение её нулями
    char message[100];
    int serv_addr_len = sizeof(serv_addr);
    for(int i = 0; i < 2; i++){
        bzero(message, 100);
        printf("Введите сообщение (%d из 2):\n", i + 1);
        // Ввод сообщения бользователем
        fgets(message, 100, stdin);
        message[strlen(message) - 1] = '\0';
        // Отправка серверу сообщения
        sendto(sock, message, strlen(message), 0, (const struct sockaddr *)&serv_addr, serv_addr_len);
        printf("Сообщение (%s) отправлено серверу\n", message);
    }

    close(sock);

    return 0;
}