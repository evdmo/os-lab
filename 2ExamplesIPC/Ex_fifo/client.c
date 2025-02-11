// подробное описание см. в Уч.пос.S17-72 Душутина Е.В."СПО Межпроц.взаимод.в ОС"стр.19-26

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    // Сервер уже создал каналы, открываем их
    int chan1 = open("channel1", O_RDONLY);
    if (chan1 == -1) {
        printf("Не могу открыть канал1 для чтения\n");
        exit(0);
    }

    int chan2 = open("channel2", O_WRONLY);
    if (chan2 == -1) {
        printf("Не могу открыть канал2 для записи\n");
        exit(0);
    }

    // Читаем имя файла из первого канала
    char fileName[100];
    bzero(fileName, 100);
    int res = read(chan1, fileName, 100);
    if (res <= 0) {
        printf("Не могу прочитать имя файла из канала1\n");
        exit(0);
    }

    // Открываем файл на чтение
    FILE *f = fopen(fileName, "r");
    if (!f) {
        printf("Не могу открыть файл %s\n", fileName);
        exit(0);
    }

    // Читаем из файла и пишем во второй канал
    char buf[100];
    while (!feof(f)) {
        // Читаем данные из файла
        res = fread(buf, sizeof(char), 100, f);
        // Пишем их в канал
        write(chan2, buf, res);
    }

    fclose(f);
    close(chan1);
    close(chan2); 
    printf("Клиент закончил работу.");
    return 0;
}

