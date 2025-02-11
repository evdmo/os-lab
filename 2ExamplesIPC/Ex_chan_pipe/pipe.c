// подробное описание см. в Уч.пос.S17-72 Душутина Е.В."СПО Межпроц.взаимод.в ОС"стр.16-19

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DEF_F_R "from.txt"
#define DEF_F_W "to.txt"

int main(int argc, char** argv) {
    char fileToRead[32];
    char fileToWrite[32];

    if (argc < 3) {
        printf("Используются имена файлов по умолчанию: '%s', '%s'\n", DEF_F_R, DEF_F_W);
        strcpy(fileToRead, DEF_F_R);
        strcpy(fileToWrite, DEF_F_W);
    } else {
        strcpy(fileToRead, argv[1]);
        strcpy(fileToWrite, argv[2]);
    }

    int filedes[2];

    if (pipe(filedes) < 0) {
        printf("Отец: не удалось создать канал\n");
        exit(1);
    }

    printf("Канал успешно создан\n");

    if (fork() == 0) {
        // Процесс-сын
        // Закрываем канал для чтения
        close(filedes[0]);

        FILE* f = fopen(fileToRead, "r");
        if (!f) {
            printf("Сын: не могу открыть файл %s\n", fileToRead);
            exit(1);
        }

        char buf[100];
        int res;
        while (!feof(f)) {
            // Читаем данные из файла
            res = fread(buf, sizeof(char), 100, f);
            write(filedes[1], buf, res); // Пишем их в канал
        }

        fclose(f);
        close(filedes[1]);
        return 0;
    }

    // Процесс-отец
    // Закрываем канал для записи
    close(filedes[1]);

    FILE* f = fopen(fileToWrite, "w");
    if (!f) {
        printf("Отец: не могу открыть файл %s\n", fileToWrite);
        exit(1);
    }

    char buf[100];
    int res;
    while (1) {
        bzero(buf, 100);
        res = read(filedes[0], buf, 100);
        if (!res)
            break;
        printf("Прочитано из канала: %s\n", buf);
        fwrite(buf, sizeof(char), res, f);
    }

    fclose(f);
    close(filedes[0]);
    return 0;
}

