// подробное описание см. в Уч.пос.S17-72 Душутина Е.В."СПО Межпроц.взаимод.в ОС"стр.19-26

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEF_FILENAME "testFile.txt"

// изменим программу server.c на server1 так, чтобы сервер, перед тем, как читать данные из канала, ожидал ввода пользователя

int main(int argc, char** argv) {
    char fileName[30];

    if(argc < 2) {
        printf("Используется имя файла по умолчанию '%s'\n", DEF_FILENAME);
        strcpy(fileName, DEF_FILENAME);
    }
    else
        strcpy(fileName, argv[1]);

    // Создаем два канала
    int res = mknod("channel1", S_IFIFO | 0666, 0);
    if(res) {
        printf("Не удалось создать первый канал\n");
        exit(1);
    }

    res = mknod("channel2", S_IFIFO | 0666, 0);
    if(res) {
        printf("Не удалось создать второй канал\n");
        exit(1);
    }

    // Открываем первый канал для записи
    int chan1 = open("channel1", O_WRONLY);
    if(chan1 == -1) {
        printf("Не могу открыть канал для записи\n");
        exit(0);
    }

    // Открываем второй канал для чтения
    int chan2 = open("channel2", O_RDONLY);
    if(chan2 == -1) {
        printf("Не могу открыть второй канал для чтения\n");
        exit(0);
    }

    // Пишем имя файла в первый канал
    write(chan1, fileName, strlen(fileName));
    
    // Читаем содержимое файла из второго канала
    char buf[100];
    printf("Ждём ввода клиента в канал...");
    getchar();
    for(;;) {
        bzero(buf, 100);
        res = read(chan2, buf, 100);
        if(res <= 0)
            break;
        printf("Часть файла: %s\n", buf);
    }

    close(chan1);
    close(chan2);
    unlink("channel1");
    unlink("channel2");

    return 0;
}

