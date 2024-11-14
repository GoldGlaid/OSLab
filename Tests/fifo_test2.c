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
    int sum = 0;

    //Открываем файловый дескриптор только на чтение
    int fd_read = open(NAME_FIFO, O_RDONLY);
    if (fd_read == -1) {
        printf("Fail with opening fd :(\n");
        return FD_ERROR;
    }

    //Считываем в файловый дескриптор наш массив чисел
    //arr - указывает на 1 элемент массива в памяти, следовательно,
    // можно считать сразу весь массив зная его длину по памяти
    if (read(fd_read, arr, sizeof(int) * sizeArr) == -1) {
        printf("Read fail :(\n");
        close(fd_read);
        return READ_ERROR;
    }
    close(fd_read);

    //печатаем массив
    for (int i = 0; i < sizeArr; ++i) {
        int temp_sum = sum + arr[i];
        printf("%d + %d = %d\n", sum, arr[i], temp_sum);
        sum += arr[i];
    }

    int fd_write = open(NAME_FIFO, O_WRONLY);
    if (fd_write == -1) {
        printf("Fail with opening fd :(\n");
        return FD_ERROR;

    }
    if (write(fd_write, &sum, sizeof(sum)) == -1) {
        printf("Write fail :(\n");
        close(fd_write);
        return WRITE_ERROR;
    }
    close(fd_write);

    return 0;
}