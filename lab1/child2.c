#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; ++i) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main(int argc, char *argv[]) {
    //файл только на запись был создан в parent, но как его сюда передать?

    char *end;
    int fd = strtol(argv[1], &end, 10);

    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        buffer[bytes_read] = '\0';
        reverse_string(buffer);

        write(STDIN_FILENO, buffer, strlen(buffer));
        write(fd, buffer, strlen(buffer));
    }

    close(fd);
    close(STDIN_FILENO);
    return 0;
}
