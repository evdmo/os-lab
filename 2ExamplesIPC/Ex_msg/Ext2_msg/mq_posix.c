//POSIX mq
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

// Вспомогательная функция для проверки успешной отработки функций с целью уменьшения объема кода
int success(int value) {
	if (value == -1) {
		perror("Failed success check");
		exit(1);
	} else {
		return value;
	}
}

int main(int argc, char** argv) {
	const char* QUEUE_NAME = "/my_queue";
	// Sender
	if (argc > 1 && argv[1][0] == '1') { // с вх.аргументом 1 запускаем процесс Sender, записывающий сообщения в очередь
		if (mq_unlink(QUEUE_NAME) != -1) { // удаляем очередь, если она уже существует
			printf("The queue already existed and was removed\n");
		}
		
		// Открываем очередь на запись или создаем очередь, если ее не было
		mqd_t queue = success(mq_open(QUEUE_NAME, O_WRONLY | O_CREAT | O_EXCL, 0666, NULL)); 
		printf("Queue for sending, enter message or enter \"exit\" to stop\n");
		
		// Запрашиваем ввод пользователя, пока не будет введено "exit"
		char buffer[1024];
		int i = 0;
		int prio = 0;
		while (1) {
			printf("Enter next message: ");
			if (!fgets(buffer, 1023, stdin)) break;
			
			size_t len = strlen(buffer);
			if (buffer[len - 1] == '\n') buffer[len - 1] = '\0'; // удаляем символ переноса строки
			
			success(mq_send(queue, buffer, len, prio));
			
			if (strcmp(buffer, "exit") == 0) { // получена команда останова 
				break;
			}
			
			if (i++ >= prio) { // увеличиваем приоритет сообщений
				prio++;
				i = 0;
			}
		}
		
		success(mq_close(queue)); // закрываем дескриптор очереди (и завершаем процесс Sender)
		printf("Sender finished successfully\n");

	} 
	  // Receiver
	  else if (argc > 1 && argv[1][0] == '2') { // с вх. аргументом 2 запускаем процесс Receiver, читающий из очереди сообщений
		mqd_t queue = success(mq_open(QUEUE_NAME, O_RDONLY)); // открываем очередь только на чтение
		
		struct mq_attr attr = { 0 };
		success(mq_getattr(queue, &attr)); // получаем текущие параметры очереди сообщений
		
		// Читаем все сообщения из очереди
		char buffer[16*1024];
		int prio;
		int msg_count = attr.mq_curmsgs;
		while (msg_count--) {
			success(mq_receive(queue, buffer, 16*1024, &prio));
			printf("Received message (priority %d): %s\n", prio, buffer);
		}
		
		success(mq_close(queue)); // закрываем дескриптор очереди
		success(mq_unlink(QUEUE_NAME)); // удаляем очередь 
		printf("Receiver finished successfully\n");
	} else {
		printf("Expected first argument: either of 1 or 2\n");
	}
	
	return 0;
}