#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>


#define NAME_FIFO "sum"

typedef enum {
    MKFIFO_ERROR = 1,
    FD_ERROR,
    WRITE_ERROR,
    READ_ERROR,
} state;


int main(int argc, char *argv[]) {
    int sizeArr = 5;
    int arr[sizeArr];

    //Заносим в массив рандомные числа
    srand(time(NULL));
    for (int i = 0; i < sizeArr; ++i) {
        arr[i] = rand() % 1000;
    }

    //Открываем файловый дескриптор только на запись
    int fd_write = open(NAME_FIFO, O_WRONLY);
    if (fd_write == -1) {
        if (mkfifo(NAME_FIFO, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH) == -1) {
            printf("MKFIFO_ERROR\n");
            return MKFIFO_ERROR;
        }
    }


    //Записываем в файловый дескриптор наш массив чисел
    //arr - указывает на 1 элемент массива в памяти, следовательно,
    // можно записать сразу весь массив зная его длину по памяти
    if (write(fd_write, arr, sizeof(int) * sizeArr) == -1) {
        printf("Write fail :(\n");
        close(fd_write);
        return WRITE_ERROR;
    }
    close(fd_write);

    //Открываем файловый дескриптор только на чтение
    int fd_read = open(NAME_FIFO, O_RDONLY);
    if (fd_read == -1) {
        printf("Fail with opening fd :(\n");
        return FD_ERROR;
    }

    //Читаем сумму из дескриптора
    int sum = 0;
    if (read(fd_read, &sum, sizeof(sum)) == -1) {
        printf("Read fail :(\n");
        return READ_ERROR;
    }
    close(fd_write);

    printf("Final sum: %d\n", sum);
    return 0;
}