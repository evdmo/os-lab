// подробное описание см.в Уч.пос.(S17-72) Душутина Е.В."СПО Межпроц.взаимод.в ОС"с.45-51

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEF_PORT 8888
#define DEF_IP "127.0.0.1"

int main(int argc, char** argv) {
    char* addr;
    int port;
    char* readbuf;
    
    if (argc < 3) {
        printf("Используется порт по умолчанию: %d\n", DEF_PORT);
        port = DEF_PORT;
    } else
        port = atoi(argv[2]);
    
    if (argc < 2) {
        printf("Используется адрес по умолчанию: %s\n", DEF_IP);
        addr = DEF_IP;
    } else
        addr = argv[1];
    
    // Создаем сокет 
    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port = htons(port);
    peer.sin_addr.s_addr = inet_addr(addr);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Не удается создать сокет\n");
        exit(1);
    }
    
    // Присоединяемся к серверу
    int res = connect(sock, (struct sockaddr *)&peer, sizeof(peer));
    if (res) {
        perror("Не удается подключиться к серверу:");
        exit(1);
    }
    
    // Основной цикл программы
    char buf[100];
    for (;;) {
        printf("Введите запрос (пустая строка для выхода)\n");
        bzero(buf, 100);
        fgets(buf, 100, stdin);
        buf[strlen(buf) - 1] = '\0';
        if (strlen(buf) == 0) {
            printf("Пока-пока\n");
            return 0;
        }
        res = sendFix(sock, buf, 0);
        if (res <= 0) {
            perror("Ошибка отправки:");
            exit(1);
        }
        bzero(buf, 100);
        res = readFix(sock, buf, 100, 0);
        if (res <= 0) {
            perror("Ошибка при получении:");
            exit(1);
        }
        printf("Ответ сервера: %s\n", buf);
    }
    
    return 0;
}

int readFix(int sock, char* buf, int bufSize, int flags) {
    // Читаем "заголовок" - сколько байт составляет наше сообщение
    unsigned msgLength = 0;
    int res = recv(sock, &msgLength, sizeof(unsigned), flags|MSG_WAITALL);
    if (res <= 0) return res;
    if (res > bufSize) {
        printf("Получено больше данных, чем мы можем обработать, завершение\n");
        exit(1);
    }
    // Читаем само сообщение
    return recv(sock, buf, msgLength, flags | MSG_WAITALL);
}

int sendFix(int sock, char* buf, int flags) {
    // Число байт в сообщении
    unsigned msgLength = strlen(buf);
    int res = send(sock, &msgLength, sizeof(unsigned), flags);
    if (res <= 0) return res;
    send(sock, buf, msgLength, flags);
}

