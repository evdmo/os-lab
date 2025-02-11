#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

void *threadFunction(void *arg) {
    int thread_id = *((int *)arg);
    printf("Нить %d началась\n", thread_id);

    while(1) {
        sleep(1);
        printf("Нить %d работает...\n", thread_id);
    }

    printf("Нить %d завершилась\n", thread_id);
    pthread_exit(NULL);
}

void signalHandler(int signal) {
    if (signal == SIGUSR1) {
        printf("Получен SIGUSR1 сигнал в первой нити\n");
        // Завершение процесса при получении сигнала SIGUSR1
        exit(EXIT_SUCCESS);
    }
}

int main() {
    pthread_t threads[2];
    int thread_ids[2];
    int i;

    // Установка обработчика сигнала SIGUSR1
    signal(SIGUSR1, signalHandler);

    // Создание и запуск нитей
    for (i = 0; i < 2; i++) {
        thread_ids[i] = i + 1;

        if (pthread_create(&threads[i], NULL, threadFunction, &thread_ids[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i + 1);
            exit(EXIT_FAILURE);
        }
    }

    // Подождем 5 секунд
    sleep(5);

    // Отправка сигнала SIGUSR1 второй нити
    pthread_kill(threads[1], SIGUSR1);

    // Ожидание завершения всех нитей
    for (i = 0; i < 2; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i + 1);
            exit(EXIT_FAILURE);
        }
    }

    printf("Все нити завершены...\n");

    return 0;
}
