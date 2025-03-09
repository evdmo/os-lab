#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	// Выводим информацию о переданных в программу аргументах
	printf("Started son, number of arguments: %d\nArguments: ", argc);
	for (int i = 0; i < argc; i++) {
		printf("%s, ", argv[i]);
	}
	
	// Выведем значение переменной среды HOME
	printf("\n$HOME = ");
	fflush(stdout);
	system("echo $HOME");
	
	if (argc > 1) {
		printf("Finished son %s\n", argv[1]);
	}
	
    return 0;
}
