// подробное описание см. в Уч.пос.S17-72 "СПО Межпроц.взаимод.в ОС"с.45-51

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define DEF_PORT 8888
#define DEF_IP "127.0.0.1"

// Функция обработки клиента
void* clientHandler(void* args) {
    int sock = (int)args;
    char buf[100];
    int res = 0;
    
    for (;;) {
        bzero(buf, 100);
        res = readFix(sock, buf, 100, 0);
        if (res <= 0) {
            perror("Не удается принять данные от клиента, завершение потока\n");
            pthread_exit(NULL);
        }
        printf("Клиент отправил: %s\n", buf);
        res = sendFix(sock, buf, 0);
        if (res <= 0) {
            perror("Ошибка отправки");
            pthread_exit(NULL);
        }
    }
}

int main(int argc, char** argv) {
    int port = 0;
    
    if (argc < 2) {
        printf("Используется порт по умолчанию: %d\n", DEF_PORT);
        port = DEF_PORT;
    } else
        port = atoi(argv[1]);
    
    struct sockaddr_in listenerInfo;
    listenerInfo.sin_family = AF_INET;
    listenerInfo.sin_port = htons(port);
    listenerInfo.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("Не удается создать сокет для прослушивания: ");
        exit(1);
    }
    
    int res = bind(listener, (struct sockaddr *)&listenerInfo, sizeof(listenerInfo));
    if (res < 0) {
        perror("Не удается привязать сокет");
        exit(1);
    }
    
    // Слушаем входящие соединения
    res = listen(listener, 5);
    if (res) {
        perror("Ошибка при прослушивании:");
        exit(1);
    }
    
    // Основной цикл работы
    for (;;) {
        int client = accept(listener, NULL, NULL);
        pthread_t thrd;
        res = pthread_create(&thrd, NULL, clientHandler, (void *)(client));
        if (res) {
            printf("Ошибка при создании нового потока\n");
        }
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
    // Отправляем количество байт в сообщении
    unsigned msgLength = strlen(buf);
    int res = send(sock, &msgLength, sizeof(unsigned), flags);
    if (res <= 0) return res;
    return send(sock, buf, msgLength, flags);
}

